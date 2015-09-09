#include "App/HekateMessageHandler.h"

#include "plog/Log.h"

namespace Hekate
{
namespace App
{

HekateMessageHandler::HekateMessageHandler(IHekate * const pApp) :
    m_pApp{ pApp }
{
}

const bool HekateMessageHandler::OnAddHookMessage(const Protobuf::Proto::Client::AddHookMessage &msg)
{
    LOG_VERBOSE << "Handing add hook message.";

    return m_pApp->AddHook(msg.name());
}

const bool HekateMessageHandler::OnRemoveHookMessage(const Protobuf::Proto::Client::RemoveHookMessage &msg)
{
    LOG_VERBOSE << "Handling remove hook message.";

    return m_pApp->RemoveHook(msg.name());
}

const bool HekateMessageHandler::OnAddFilterMessage(const Protobuf::Proto::Client::AddFilterMessage &msg)
{
    LOG_VERBOSE << "Handling add filter message.";

    return m_pApp->AddFilter(msg);
}

const bool HekateMessageHandler::OnRemoveFilterMessage(const Protobuf::Proto::Client::RemoveFilterMessage &msg)
{
    LOG_VERBOSE << "Handling remove filter message.";

    return m_pApp->RemoveFilter(msg.filter_id());
}

const bool HekateMessageHandler::OnContinueMessage()
{
    LOG_VERBOSE << "Handling continue message.";

    return m_pApp->Continue();
}

const bool HekateMessageHandler::OnReplayMessage(const Protobuf::Proto::Client::ReplayMessage &msg)
{
    LOG_VERBOSE << "Handling replay message.";

    return m_pApp->Replay(msg);
}

}
}