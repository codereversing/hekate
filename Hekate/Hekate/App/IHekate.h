#pragma once

#include "Common/SafeObject.h"
#include "Filter/HekateFilter.h"
#include "Hook/HookEngine.h"
#include "IPC/IPCBase.h"
#include "Protobuf/Message/IMessageHandler.h"
#include "Protobuf/Proto/Server/HekateServerProto.pb.h"
#include "Winsock/WinsockFunction.h"
#include "Winsock/Hooks/WinsockHooks.h"

#include <map>
#include <memory>
#include <string>

namespace Hekate
{
namespace App
{

class IHekate
{
public:
    friend class Winsock::Hooks::WinsockHooks;

    IHekate() = default;
    ~IHekate() = default;

    virtual const bool AddHook(const std::string &functionName) = 0;
    virtual const bool RemoveHook(const std::string &functionName) = 0;
    virtual const bool AddFilter(const Protobuf::Proto::Client::AddFilterMessage &msg) = 0;
    virtual const bool RemoveFilter(const int filterId) = 0;
    virtual const bool Continue() = 0;
    virtual const bool Pause() = 0;
    virtual const bool Replay(const Protobuf::Proto::Client::ReplayMessage &msg) = 0;

protected:

    virtual const DWORD_PTR GetCallAddress(const Winsock::WinsockFunction function) = 0;
    virtual const bool SendHekateMessage(const Protobuf::Proto::Server::HekateMessage &msg) = 0;
    virtual const Filter::FilteredInput ApplyFilter(const char * const bytes, const int length) = 0;

    virtual void InitializeAddressMap() = 0;
    virtual void InitializeHookEngine() = 0;
    virtual void InitializeIPC() = 0;
    virtual void InitializeMessageHandler() = 0;
    virtual void InitializeFilter() = 0;
    virtual void InitializeWinsockHooks() = 0;
    virtual void InitializeWaitEvent() = 0;
};

}
}