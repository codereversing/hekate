#include "Hook/Platform/InlinePlatformHook.h"

#include "Capstone/CapstoneDll.h"
#include "Common/Common.h"

#include "plog/Log.h"

#include <TlHelp32.h>

namespace Hekate
{
namespace Hook
{
namespace Platform
{

InlinePlatformHook::InlinePlatformHook(const cs_arch architecture, const cs_mode mode,
    const size_t hookSizeBytes) :
    m_architecture{ architecture },
    m_mode{ mode },
    m_hookSizeBytes{ hookSizeBytes },
    m_bCanHook { true }
{
}

const DWORD_PTR InlinePlatformHook::Hook(const DWORD_PTR originalAddress, const DWORD_PTR hookAddress,
    size_t &jumpOffset)
{
    LOG_DEBUG << "Attempting to hook function at 0x" << std::hex << originalAddress;
    LOG_DEBUG << "Hook will point to 0x" << std::hex << hookAddress;

    const DWORD protections = SetPagePermissions(originalAddress, PAGE_EXECUTE_READWRITE);
    m_bCanHook = (protections != 0);

    LPVOID lpTrampolineAddress = AllocateTrampoline();

    Capstone::ScopedCapstoneInstructions instructions;
    Disassemble(originalAddress, instructions);

	size_t bytesTotal = 0;
    CopyToTrampoline(lpTrampolineAddress, instructions, bytesTotal);

    bool bSuccess = true;
    if (m_bCanHook)
    {
        bSuccess = SuspendThreads();
        bSuccess &= HookImpl(originalAddress, hookAddress, (DWORD_PTR)lpTrampolineAddress + bytesTotal, bytesTotal);
        bSuccess &= ResumeThreads();
        if (!bSuccess)
        {
            LOG_ERROR << "Problem occurred during hook installation. See error log for details.";
        }
    }
    else
    {
        LOG_ERROR << "Did not attempt to install hook because an error was detected.";
        bSuccess = false;
    }

    if (!bSuccess)
    {
        if (lpTrampolineAddress != nullptr)
        {
            LOG_WARNING << "Freeing memory that was allocated for trampoline but was not used";

            bSuccess = BOOLIFY(VirtualFree(lpTrampolineAddress, PAGE_SIZE, MEM_FREE));
            if (!bSuccess)
            {
                LOG_ERROR << "[Possible memory leak]: Could not successfully free memory allocated for trampoline";
            }

            lpTrampolineAddress = nullptr;
        }
    }

    (void)SetPagePermissions(originalAddress, protections);

    bSuccess = BOOLIFY(FlushInstructionCache(GetCurrentProcess(), (LPCVOID)originalAddress, PAGE_SIZE));
    if (!bSuccess)
    {
        LOG_ERROR << "Could not flush instruction cache. Hook may not be properly installed. "
            << "Error = 0x" << std::hex << GetLastError();
    }

    jumpOffset = bytesTotal;
    return (DWORD_PTR)lpTrampolineAddress;
}

const bool InlinePlatformHook::Unhook(const DWORD_PTR originalAddress, const DWORD_PTR trampolineAddress,
    const size_t jumpOffset)
{
    LOG_DEBUG << "Attempting to unhook function at 0x" << std::hex << originalAddress;

    const DWORD protections = SetPagePermissions(originalAddress, PAGE_EXECUTE_READWRITE);
    bool bSuccess = (protections != 0);
    if (bSuccess)
    {
        bSuccess &= SuspendThreads();
        bSuccess &= UnhookImpl(originalAddress, trampolineAddress, jumpOffset);
        bSuccess &= ResumeThreads();
        if (!bSuccess)
        {
            LOG_ERROR << "Problem occurred during hook removal. See error log for details.";
        }
    }
    else
    {
        LOG_ERROR << "Did not attempt to unhook function at 0x" << std::hex << originalAddress
            << " due to bad page permissions.";
    }
    (void)SetPagePermissions(originalAddress, protections);

    const LPVOID lpTrampolineAddress = (LPVOID)trampolineAddress;
    if (lpTrampolineAddress != nullptr)
    {
        bSuccess &= BOOLIFY(VirtualFree(lpTrampolineAddress, 0, MEM_RELEASE));
        if (!bSuccess)
        {
            LOG_ERROR << "Could not free memory for trampoline at 0x"
                << std::hex << trampolineAddress;
            LOG_ERROR << "Error = 0x" << std::hex << GetLastError();
        }
    }
    else
    {
        LOG_WARNING << "Went to unhook a function at 0x" << std::hex << originalAddress
            << " but the trampoline address was null.";
    }

    if (bSuccess)
    {
        LOG_DEBUG << "Successfully removed hook from 0x" << std::hex << originalAddress;
    }

    (void)FlushInstructionCache(GetCurrentProcess(), (LPCVOID)originalAddress, PAGE_SIZE);
    bSuccess = BOOLIFY(FlushInstructionCache(GetCurrentProcess(), (LPCVOID)originalAddress, PAGE_SIZE));
    if (!bSuccess)
    {
        LOG_ERROR << "Could not flush instruction cache. Hook may not be properly uninstalled. "
            << "Error = 0x" << std::hex << GetLastError();
    }

    return bSuccess;
}

const bool InlinePlatformHook::SuspendThreads() const
{
    auto vecHandles = GetThreads();
    if (vecHandles.size() > 0)
    {
        for (auto &thread : vecHandles)
        {
            (void)SuspendThread(thread());
        }
    }
    else
    {
        LOG_ERROR << "No threads in thread list to suspend.";
        return false;
    }

    return true;
}

const bool InlinePlatformHook::ResumeThreads() const
{
    auto vecHandles = GetThreads();
    if (vecHandles.size() > 0)
    {
        for (auto &thread : vecHandles)
        {
            (void)ResumeThread(thread());
        }
    }
    else
    {
        LOG_ERROR << "No threads in thread list to resume.";
        return false;
    }

    return true;
}

std::vector<Common::SafeHandle> InlinePlatformHook::GetThreads() const
{
    std::vector<Common::SafeHandle> vecHandles;

    Common::SafeHandle snapshot(CreateToolhelp32Snapshot(TH32CS_SNAPTHREAD, GetCurrentProcessId()));
    if (!snapshot.IsValid())
    {
        LOG_ERROR << "Could not create snapshot to enumerate process threads. "
            << "Error = 0x" << std::hex << GetLastError();
    }
    else
    {
        THREADENTRY32 threadEntry = { 0 };
        threadEntry.dwSize = sizeof(THREADENTRY32);
        if (!BOOLIFY(Thread32First(snapshot(), &threadEntry)))
        {
            LOG_ERROR << "Could not begin traversing thread list. "
                << "Error = 0x" << std::hex << GetLastError();
        }
        else
        {
            do
            {
                if (threadEntry.th32OwnerProcessID == GetCurrentProcessId() &&
                    threadEntry.th32ThreadID != GetCurrentThreadId())
                {
                    LOG_VERBOSE << "Found thread in target process " <<
                        "id = " << std::hex << threadEntry.th32ThreadID;
                    Common::SafeHandle hThread = OpenThread(THREAD_SUSPEND_RESUME,
                        FALSE, threadEntry.th32ThreadID);
                    if (!hThread.IsValid())
                    {
                        LOG_ERROR << "Unable to open handle to thread 0x"
                            << std::hex << threadEntry.th32ThreadID
                            << " Error = 0x" << std::hex << GetLastError();
                    }
                    else
                    {
                        LOG_VERBOSE << "Inserting handle to thread 0x"
                            << std::hex << threadEntry.th32ThreadID << " into thread list.";
                        vecHandles.emplace_back(std::move(hThread));
                    }

                } 
            } while (BOOLIFY(Thread32Next(snapshot(), &threadEntry)));
        }
    }

    return vecHandles;
}

LPVOID InlinePlatformHook::AllocateTrampoline()
{
    const LPVOID trampolineAddress = VirtualAlloc(nullptr, PAGE_SIZE, 
       MEM_RESERVE | MEM_COMMIT, PAGE_EXECUTE_READWRITE);
    if (trampolineAddress == nullptr)
    {
        LOG_ERROR << "Unable to allocate memory for trampoline. "
            << "Error = 0x" << std::hex << GetLastError();
        m_bCanHook = false;
    }
    else
    {
        LOG_DEBUG << "Allocated trampoline memory at 0x" << std::hex << trampolineAddress;
    }

    return trampolineAddress;
}

void InlinePlatformHook::Disassemble(const DWORD_PTR originalAddress, Capstone::ScopedCapstoneInstructions &instructions)
{
    Capstone::ScopedCapstoneHandle handle(m_architecture, m_mode);

    LOG_DEBUG << "Disassembing instructions starting at address 0x" << std::hex << originalAddress;

    *instructions.CountPtr() = Capstone::cs_disasm(handle(), (const uint8_t *)originalAddress,
        0x20, 0, 0, instructions.InstructionsPtr());
    if (instructions.Count() <= 0)
    {
        LOG_ERROR << "Unable to disassemble code at address 0x" << std::hex << originalAddress;
        m_bCanHook = false;
    }
}

void InlinePlatformHook::CopyToTrampoline(const LPVOID trampolineAddress,
    Capstone::ScopedCapstoneInstructions &instructions, size_t &bytesTotal) const
{
    for (size_t i = 0; i < instructions.Count() && bytesTotal < m_hookSizeBytes; ++i)
    {
        (void)memcpy(((unsigned char *)trampolineAddress) + bytesTotal,
            instructions.Instructions()[i].bytes, instructions.Instructions()[i].size);
        bytesTotal += instructions.Instructions()[i].size;
    }
}

const DWORD InlinePlatformHook::SetPagePermissions(const DWORD_PTR address, const DWORD permissions) const
{
    DWORD oldProtections = 0;
    const bool bSuccess = BOOLIFY(VirtualProtect((LPVOID)address, PAGE_SIZE, permissions, &oldProtections));
    if (!bSuccess)
    {
        LOG_ERROR << "Could not change page protections at address 0x" << std::hex << address;
        LOG_ERROR << "Error = 0x" << GetLastError();

        return 0;
    }

    return oldProtections;
}

const bool InlinePlatformHook::CanHook() const
{
    return m_bCanHook;
}

}
}
}