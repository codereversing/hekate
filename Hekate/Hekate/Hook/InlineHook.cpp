#include "Hook/InlineHook.h"

#include "Common/Common.h"

#include "plog/Log.h"

namespace Hekate
{
namespace Hook
{

std::mutex InlineHook::m_hookMutex;

InlineHook::InlineHook(const DWORD_PTR originalAddress, const DWORD_PTR hookAddress) :
    HookBase(HookType::eInlineHook, originalAddress, hookAddress),
    m_trampolineAddress{ 0 },
    m_jumpOffset { 0 }
{
#ifdef _M_IX86
    m_pPlatformHook = std::unique_ptr<Platform::InlinePlatformHook_x86>(new Platform::InlinePlatformHook_x86);
#elif defined _M_AMD64
    m_pPlatformHook = std::unique_ptr<Platform::InlinePlatformHook>(new Platform::InlinePlatformHook_x64);
#else
#error "Unsupported platform"
#endif
}

InlineHook::InlineHook(InlineHook &&obj) : HookBase(HookType::eInlineHook, obj.m_originalAddress, obj.m_hookAddress)
{
    *this = std::move(obj);
}

InlineHook &InlineHook::operator=(InlineHook &&obj)
{
    m_trampolineAddress = obj.m_trampolineAddress;
    m_jumpOffset = obj.m_jumpOffset;

    obj.m_trampolineAddress = 0;
    obj.m_jumpOffset = 0;

    return *this;
}

InlineHook::~InlineHook()
{
    LOG_DEBUG << "Destroying hook. 0x" << std::hex << m_hookAddress
        << " --> 0x" << std::hex << m_originalAddress;

   if (IsHooked())
    {
        (void)RemoveImpl();
    }
}

const bool InlineHook::InstallImpl()
{
    std::lock_guard<std::mutex> lock(m_hookMutex);

    m_trampolineAddress = m_pPlatformHook->Hook(OriginalAddress(), HookAddress(),
        m_jumpOffset);

    return m_trampolineAddress != 0;
}

const bool InlineHook::RemoveImpl()
{
    std::lock_guard<std::mutex> lock(m_hookMutex);

    auto success = m_pPlatformHook->Unhook(OriginalAddress(), TrampolineAddress(), m_jumpOffset);

    return success;
}

const DWORD_PTR InlineHook::TrampolineAddress() const
{
    return m_trampolineAddress;
}

}
}