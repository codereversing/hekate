#pragma once

#include "App/IHekate.h"
#include "Protobuf/Message/IMessageHandler.h"
#include "Protobuf/Proto/Client/HekateClientProto.pb.h"

namespace Hekate
{
namespace App
{

class HekateMessageHandler final : public Protobuf::Message::IMessageHandler
{
public:
    HekateMessageHandler() = delete;

    HekateMessageHandler(const HekateMessageHandler &copy) = delete;
    HekateMessageHandler &operator=(const HekateMessageHandler &copy) = delete;

    HekateMessageHandler(IHekate * const pApp);
    ~HekateMessageHandler() = default;

private:
    IHekate * const m_pApp;

    const bool OnAddHookMessage(const Protobuf::Proto::Client::AddHookMessage &msg) override;
    const bool OnRemoveHookMessage(const Protobuf::Proto::Client::RemoveHookMessage &msg) override;
    const bool OnAddFilterMessage(const Protobuf::Proto::Client::AddFilterMessage &msg) override;
    const bool OnRemoveFilterMessage(const Protobuf::Proto::Client::RemoveFilterMessage &msg) override;
    const bool OnContinueMessage() override;
    const bool OnReplayMessage(const Protobuf::Proto::Client::ReplayMessage &msg) override;
};

}
}