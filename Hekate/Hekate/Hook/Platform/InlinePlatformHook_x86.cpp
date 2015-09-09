#ifdef _M_IX86

#include "Hook/Platform/InlinePlatformHook_x86.h"

#include "Common/Common.h"

#include <array>

#include "plog/Log.h"

namespace Hekate
{
namespace Hook
{
namespace Platform
{

InlinePlatformHook_x86::InlinePlatformHook_x86() : InlinePlatformHook(CS_ARCH_X86, CS_MODE_32, HOOK_SIZE)
{
}

const bool InlinePlatformHook_x86::HookImpl(const DWORD_PTR originalAddress, const DWORD_PTR hookAddress,
    const DWORD_PTR trampolineAddress, const size_t overwriteSize)
{
    std::array<unsigned char, HOOK_SIZE * 2> jmpHookBytes =
    {
        0x68, 0xA1, 0xA2, 0xA3, 0xA4,  /*push, 0xA1A2A3A4*/
        0xC3,                          /*jmp eax*/
        0xCC, 0xCC, 0xCC, 0xCC, 0xCC,
        0xCC                           /*int 3*/
    };

    (void)memcpy(&jmpHookBytes.data()[1], &hookAddress, sizeof(DWORD_PTR));

    //Write hook to function
    (void)memcpy((void *)originalAddress, jmpHookBytes.data(), overwriteSize);

    //Write bytes to jump back to trampoline
    const DWORD_PTR dwJumpBackAddress = originalAddress + overwriteSize;
    (void)memcpy(&jmpHookBytes.data()[1], &dwJumpBackAddress, sizeof(DWORD_PTR));

    (void)memcpy((void *)trampolineAddress, jmpHookBytes.data(), jmpHookBytes.size());

    return true;
}

const bool InlinePlatformHook_x86::UnhookImpl(const DWORD_PTR originalAddress,
    const DWORD_PTR trampolineAddress, const size_t jumpOffset)
{
    (void)memcpy((void *)originalAddress, (const void *)trampolineAddress, jumpOffset);

    return true;
}

}
}
}

#endif
