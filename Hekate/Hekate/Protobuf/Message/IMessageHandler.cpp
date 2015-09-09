#include "Protobuf/Message/IMessageHandler.h"

#include "plog/Log.h"

namespace Hekate
{
namespace Protobuf
{
namespace Message
{

const bool IMessageHandler::Parse(const Proto::Client::HekateClientMessage &msg)
{
    bool bRet = false;

    const auto &clientmsg = msg.clientmessage();
    const auto &type = clientmsg.type();
    switch (type)
    {
    case Protobuf::Proto::Client::MessageType::ADD_HOOK:
        if (!clientmsg.has_add_hook())
        {
            LOG_ERROR << "Received empty add hook message.";
        }
        else
        {
            bRet = OnAddHookMessage(msg.clientmessage().add_hook());
        }
        break;
    case Protobuf::Proto::Client::MessageType::REMOVE_HOOK:
        if (!clientmsg.has_remove_hook())
        {
            LOG_ERROR << "Received empty remove hook message.";
        }
        else
        {
            bRet = OnRemoveHookMessage(msg.clientmessage().remove_hook());
        }
        break;
    case Protobuf::Proto::Client::MessageType::ADD_FILTER:
        if (!clientmsg.has_add_filter())
        {
            LOG_ERROR << "Received empty add filter message.";
        }
        else
        {
            bRet = OnAddFilterMessage(msg.clientmessage().add_filter());
        }
        break;
    case Protobuf::Proto::Client::MessageType::REMOVE_FILTER:
        if (!clientmsg.has_remove_filter())
        {
            LOG_ERROR << "Received empty remove filter message.";
        }
        else
        {
            bRet = OnRemoveFilterMessage(msg.clientmessage().remove_filter());
        }
        break;
    case Protobuf::Proto::Client::MessageType::CONTINUE:
        if (!clientmsg.has_continue_())
        {
            LOG_ERROR << "Received empty continue message.";
        }
        else
        {
            bRet = OnContinueMessage();
        }
        break;
    case Protobuf::Proto::Client::MessageType::REPLAY:
        if (!clientmsg.has_replay())
        {
            LOG_ERROR << "Received empty reply message.";
        }
        else
        {
            bRet = OnReplayMessage(msg.clientmessage().replay());
        }
        break;
    }

    return bRet;
}

}
}
}
