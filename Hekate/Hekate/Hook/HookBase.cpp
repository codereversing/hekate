#include "Hook/HookBase.h"

#include "plog/Log.h"

namespace Hekate
{
namespace Hook
{

HookBase::HookBase(const HookType &type, const DWORD_PTR originalAddress, const DWORD_PTR hookAddress) :
    m_type{ type },
    m_originalAddress{ originalAddress },
    m_hookAddress{ hookAddress },
    m_bIsHooked{ false }
{
    LOG_DEBUG << "Initializing hook. 0x" << std::hex << m_hookAddress
        << " --> 0x" << std::hex << m_originalAddress;
}

HookBase::HookBase(HookBase &&obj)
{
    *this = std::move(obj);
}

HookBase &HookBase::operator=(HookBase &&obj)
{
    m_originalAddress = obj.m_originalAddress;
    m_hookAddress = obj.m_hookAddress;
    m_bIsHooked = obj.m_bIsHooked;

    obj.m_originalAddress = 0;
    obj.m_hookAddress = 0;
    obj.m_bIsHooked = false;

    return *this;
}

const bool HookBase::Install()
{
    LOG_DEBUG << "Installing hook on 0x" << std::hex << m_originalAddress
        << " Hook address is 0x" << std::hex << m_hookAddress;

    bool bSuccess = InstallImpl();
    if (bSuccess)
    {
        m_bIsHooked = true;
        LOG_DEBUG << "Hook successfully installed. 0x" << std::hex << m_originalAddress
            << " --> 0x" << std::hex << m_hookAddress;
    }
    else
    {
        LOG_ERROR << "Function 0x" << std::hex << HookAddress() << " was set to be hooked but the "
            << "operation did not complete successfully.";
    }

    return bSuccess;
}

const bool HookBase::Remove()
{
    auto bSuccess = RemoveImpl();
    if (bSuccess)
    {
        m_bIsHooked = false;
        LOG_DEBUG << "Hook successfully uninstalled. 0x" << std::hex << m_hookAddress
            << " --> 0x" << std::hex << m_originalAddress;
    }
    else
    {
        LOG_ERROR << "Function 0x" << std::hex << HookAddress() << " was set to be unhooked but the "
            << "operation did not complete successfully.";
    }

    return bSuccess;
}

const HookType HookBase::Type() const
{
    return m_type;
}

bool HookBase::operator==(const HookBase &obj) const
{
    return m_originalAddress == obj.m_originalAddress &&
        m_hookAddress == obj.m_hookAddress;
}

bool HookBase::operator!=(const HookBase &obj) const
{
    return !(*this == obj);
}

const DWORD_PTR HookBase::OriginalAddress() const
{
    return m_originalAddress;
}

const DWORD_PTR HookBase::HookAddress() const
{
    return m_hookAddress;
}

const bool HookBase::IsHooked() const
{
    return m_bIsHooked;
}

}
}