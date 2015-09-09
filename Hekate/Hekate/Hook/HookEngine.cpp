#include "Hook/HookEngine.h"

#include "Hook/InlineHook.h"

#include "plog/Log.h"

namespace Hekate
{
namespace Hook
{

const bool HookEngine::Add(const DWORD_PTR originalAddress, const DWORD_PTR hookAddress)
{
    std::unique_ptr<HookBase> pNewHook(new InlineHook(originalAddress, hookAddress));

    if (pNewHook->Install())
    {
        LOG_DEBUG << "Hook successfully created.";

        if (pNewHook->IsHooked())
        {
            LOG_DEBUG << "Address 0x" << std::hex << originalAddress <<
                " is detected as being hooked. Adding to hook list.";
            m_vecHooks.emplace_back(std::move(pNewHook));
            return true;
        }
        else
        {
            LOG_ERROR << "Hook installed on " << std::hex << originalAddress <<
                " but is being detected as unhooked. Not adding to hook list.";
            return false;
        }
    }
    else
    {
        LOG_ERROR << "Hook on 0x" << std::hex << originalAddress <<
            " could not be installed. Not adding to hook list.";
    }

    return false;
}

const bool HookEngine::Remove(const DWORD_PTR originalAddress)
{
    size_t ulCount = Count();
    if (ulCount > 0)
    {
        const auto &pHook = Find(originalAddress);

        if (pHook != nullptr)
        {
            LOG_INFO << "Found hook at " << std::hex << originalAddress;
            m_vecHooks.erase(std::remove(std::begin(m_vecHooks), std::end(m_vecHooks),
                *pHook), std::end(m_vecHooks));
            if ((Count() + 1) != ulCount)
            {
                LOG_ERROR << "Hook was not successfully removed from list.";
                return false;
            }
            return true;
        }
        else
        {
            LOG_WARNING << "Could not find hook corresponding to "
                << std::hex << originalAddress << " in hook list";
        }
    }
    else
    {
        LOG_ERROR << "Count was zero while attempting to erase from hook list.";
    }

    return false;
}

const bool HookEngine::Exists(const DWORD_PTR originalAddress) const
{
    const auto &pHook = Find(originalAddress);

    return pHook != nullptr;
}

const std::unique_ptr<HookBase> *HookEngine::Find(const DWORD_PTR originalAddress) const
{
    const auto &pHook = std::find_if(std::begin(m_vecHooks), std::end(m_vecHooks),
        [=](const std::unique_ptr<HookBase> &hookBase)
    {
        auto pHook = hookBase.get();
        if (pHook == nullptr)
        {
            LOG_ERROR << "Hook entry was null. This shouldn't happen...";
            return false;
        }
        return hookBase.get()->OriginalAddress() == originalAddress;
    });

    return (pHook == m_vecHooks.end()) ? nullptr : &(*pHook);
}

const size_t HookEngine::Count() const
{
    return m_vecHooks.size();
}

}
}