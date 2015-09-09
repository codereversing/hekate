#pragma once


#include "App/IHekate.h"
#include "Filter/HekateFilter.h"
#include "Hook/HookEngine.h"
#include "IPC/IPCBase.h"
#include "Winsock/WinsockFunction.h"
#include "Protobuf/Message/IMessageHandler.h"
#include "Protobuf/Proto/Server/HekateServerProto.pb.h"
#include "Protobuf/Proto/Client/HekateClientProto.pb.h"

#include "plog/Log.h"

#include <map>
#include <memory>
#include <vector>

namespace Hekate
{
namespace App
{

class HekateMain final : public IHekate
{
public:
    HekateMain();
    ~HekateMain() = default;

private:
    void InitializeAddressMap() override;
    void InitializeHookEngine() override;
    void InitializeIPC() override;
    void InitializeMessageHandler() override;
    void InitializeFilter() override;
    void InitializeWinsockHooks() override;
    void InitializeWaitEvent() override;

    const bool AddHook(const std::string &functionName) override;
    const bool RemoveHook(const std::string &functionName) override;
    const bool AddFilter(const Protobuf::Proto::Client::AddFilterMessage &msg) override;
    const bool RemoveFilter(const int filterId) override;
    const bool Continue() override;
    const bool Pause() override;
    const bool Replay(const Protobuf::Proto::Client::ReplayMessage &msg) override;

    void RecvCallback(const Protobuf::Proto::Client::HekateClientMessage &msg);

    const Filter::FilteredInput ApplyFilter(const char * const bytes, const int length) override;

    const DWORD_PTR GetCallAddress(const Winsock::WinsockFunction function) override;
    const bool SendHekateMessage(const Protobuf::Proto::Server::HekateMessage &msg) override;

    std::map<Winsock::WinsockFunction, DWORD_PTR> m_mapAddress;
    std::unique_ptr<Hook::HookEngine> m_hookEngine;
    std::unique_ptr<IPC::IPCBase> m_ipc;
    std::unique_ptr<Protobuf::Message::IMessageHandler> m_msgHandler;
    std::unique_ptr<Filter::HekateFilter> m_filters;
    std::unique_ptr<Winsock::Hooks::WinsockHooks> m_winsockHooks;

    Common::SafeHandle m_waitEvent;
};

}
}
