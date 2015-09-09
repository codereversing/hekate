#ifdef _M_AMD64

#include "Hook/Platform/InlinePlatformHook_x64.h"

#include "Common/Common.h"

#include "plog/Log.h"

namespace Hekate
{
namespace Hook
{
namespace Platform
{

InlinePlatformHook_x64::InlinePlatformHook_x64() : InlinePlatformHook(CS_ARCH_X86, CS_MODE_64, HOOK_SIZE)
{
}

const bool InlinePlatformHook_x64::HookImpl(const DWORD_PTR originalAddress, const DWORD_PTR hookAddress,
	const DWORD_PTR trampolineAddress, const size_t overwriteSize)
{    
    std::array<unsigned char, HOOK_SIZE * 2> jmpBytes =
    {
        0x68, 0x44, 0x33, 0x22, 0x11,                   /*push 0x11223344*/
        0xC7, 0x44, 0x24, 0x04, 0xDD, 0xCC, 0xBB, 0xAA, /*mov [rsp+0x4], 0xAABBCCDD*/
        0xC3,                                           /*ret*/
        0xCC, 0xCC, 0xCC, 0xCC, 0xCC,
        0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC,
        0xCC                                            /*int 3*/
    };

    DWORD dwHighAddress = (hookAddress >> 32) & 0xFFFFFFFF;
    DWORD dwLowAddress = (DWORD)(hookAddress & 0xFFFFFFFF);
    (void)memcpy(&jmpBytes.data()[1], &dwLowAddress, sizeof(DWORD));
    (void)memcpy(&jmpBytes.data()[9], &dwHighAddress, sizeof(DWORD));

    //Write hook to function address
    (void)memcpy((void *)originalAddress, jmpBytes.data(), overwriteSize);

    //Write bytes to jump back to trampoline
    const DWORD_PTR dwOriginalContinueAddress = originalAddress + overwriteSize;
    dwHighAddress = (dwOriginalContinueAddress >> 32) & 0xFFFFFFFF;
    dwLowAddress = (DWORD)(dwOriginalContinueAddress & 0xFFFFFFFF);

    (void)memcpy(&jmpBytes.data()[1], &dwLowAddress, sizeof(DWORD));
    (void)memcpy(&jmpBytes.data()[9], &dwHighAddress, sizeof(DWORD));

    (void)memcpy((void *)trampolineAddress, jmpBytes.data(), jmpBytes.size());

    return true;
}

const bool InlinePlatformHook_x64::UnhookImpl(const DWORD_PTR originalAddress,
    const DWORD_PTR trampolineAddress, const size_t jumpOffset)
{
    (void)memcpy((void *)originalAddress, (const void *)trampolineAddress, jumpOffset);

    return true;
}

}
}
}

#endif
