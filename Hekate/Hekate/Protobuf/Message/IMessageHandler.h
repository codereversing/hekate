#pragma once

#include "Protobuf/Proto/Client/HekateClientProto.pb.h"

namespace Hekate
{
namespace Protobuf
{
namespace Message
{

class IMessageHandler
{
public:
    IMessageHandler() = default;
    virtual ~IMessageHandler() = default;

    const bool Parse(const Proto::Client::HekateClientMessage &msg);

private:
    virtual const bool OnAddHookMessage(const Proto::Client::AddHookMessage &msg) = 0;
    virtual const bool OnRemoveHookMessage(const Proto::Client::RemoveHookMessage &msg) = 0;
    virtual const bool OnAddFilterMessage(const Proto::Client::AddFilterMessage &msg) = 0;
    virtual const bool OnRemoveFilterMessage(const Proto::Client::RemoveFilterMessage &msg) = 0;
    virtual const bool OnContinueMessage() = 0;
    virtual const bool OnReplayMessage(const Protobuf::Proto::Client::ReplayMessage &msg) = 0;
};

}
}
}
