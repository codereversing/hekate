#include "Protobuf/Message/HekateClientMessageBuilder.h"

namespace Hekate
{
namespace Protobuf
{
namespace Message
{

Proto::Client::HekateClientMessage HekateClientMessageBuilder::CreateAddHookMessage(const std::string &name)
{
    Proto::Client::HekateClientMessage msg;
    msg.mutable_clientmessage()->set_type(Proto::Client::MessageType::ADD_HOOK);
    msg.mutable_clientmessage()->mutable_add_hook()->set_name(name);

    return msg;
}

Proto::Client::HekateClientMessage HekateClientMessageBuilder::CreateRemoveHookMessage(const std::string &name)
{
    Proto::Client::HekateClientMessage msg;
    msg.mutable_clientmessage()->set_type(Proto::Client::MessageType::REMOVE_HOOK);
    msg.mutable_clientmessage()->mutable_remove_hook()->set_name(name);

    return msg;
}

Proto::Client::HekateClientMessage HekateClientMessageBuilder::CreateByteFilterMessage(const int filterId, const bool breakOnHit,
    const char * const byteValues, const int * const bytesIndices, const int byteCount)
{
    Proto::Client::HekateClientMessage msg;
    msg.mutable_clientmessage()->set_type(Proto::Client::MessageType::ADD_FILTER);
    msg.mutable_clientmessage()->mutable_add_filter()->set_type(Proto::Client::FilterType::BYTE);
    msg.mutable_clientmessage()->mutable_add_filter()->set_filter_id(filterId);
    msg.mutable_clientmessage()->mutable_add_filter()->set_break_on_hit(breakOnHit);

    auto bytemsg = msg.mutable_clientmessage()->mutable_add_filter()->mutable_byte_filter();
    for (int i = 0; i < byteCount; ++i)
    {
        bytemsg->add_byte_value(byteValues[i]);
        bytemsg->add_byte_index(bytesIndices[i]);
    }
    bytemsg->set_byte_count(byteCount);

    return msg;
}

Proto::Client::HekateClientMessage HekateClientMessageBuilder::CreateLengthFilterMessage(const int filterId, const bool breakOnHit,
    const int lengthValue, Proto::Client::LengthType type)
{
    Proto::Client::HekateClientMessage msg;
    msg.mutable_clientmessage()->set_type(Proto::Client::MessageType::ADD_FILTER);
    msg.mutable_clientmessage()->mutable_add_filter()->set_type(Proto::Client::FilterType::LENGTH);
    msg.mutable_clientmessage()->mutable_add_filter()->set_filter_id(filterId);
    msg.mutable_clientmessage()->mutable_add_filter()->set_break_on_hit(breakOnHit);

    auto lengthmsg = msg.mutable_clientmessage()->mutable_add_filter()->mutable_length_filter();
    lengthmsg->set_length_type(type);
    lengthmsg->set_length_value(lengthValue);

    return msg;
}

Proto::Client::HekateClientMessage HekateClientMessageBuilder::CreateSubstringFilterMessage(const int filterId, const bool breakOnHit,
    const char * const match, const int length)
{
    Proto::Client::HekateClientMessage msg;
    msg.mutable_clientmessage()->set_type(Proto::Client::MessageType::ADD_FILTER);
    msg.mutable_clientmessage()->mutable_add_filter()->set_type(Proto::Client::FilterType::SUBSTRING);
    msg.mutable_clientmessage()->mutable_add_filter()->set_filter_id(filterId);
    msg.mutable_clientmessage()->mutable_add_filter()->set_break_on_hit(breakOnHit);

    auto substringmsg = msg.mutable_clientmessage()->mutable_add_filter()->mutable_string_filter();
    substringmsg->set_match(match, length);
    substringmsg->set_length(length);

    return msg;
}

void HekateClientMessageBuilder::AddSubstituteMessage(Proto::Client::HekateClientMessage &msg, const char * const replacementBytes,
    const int * const replacementIndices, const int byteCount)
{
    auto substitutemsg = msg.mutable_clientmessage()->mutable_add_filter()->mutable_substitute();

    for (int i = 0; i < byteCount; ++i)
    {
        substitutemsg->add_byte_value(replacementBytes[i]);
        substitutemsg->add_byte_index(replacementIndices[i]);
    }
    substitutemsg->set_byte_count(byteCount);
}

void HekateClientMessageBuilder::AddReplacementMessage(Proto::Client::HekateClientMessage &msg, const char * const buffer,
    const int length)
{
    auto replacementmsg = msg.mutable_clientmessage()->mutable_add_filter()->mutable_replacement();

    replacementmsg->set_buffer(buffer, length);
    replacementmsg->set_length(length);
}

Proto::Client::HekateClientMessage HekateClientMessageBuilder::CreateRemoveFilterMessage(const int filterId)
{
    Proto::Client::HekateClientMessage msg;
    msg.mutable_clientmessage()->set_type(Proto::Client::MessageType::REMOVE_FILTER);
    msg.mutable_clientmessage()->mutable_remove_filter()->set_filter_id(filterId);

    return msg;
}

Proto::Client::HekateClientMessage HekateClientMessageBuilder::CreateContinueMessage()
{
    Proto::Client::HekateClientMessage msg;
    msg.mutable_clientmessage()->set_type(Proto::Client::MessageType::CONTINUE);
    msg.mutable_clientmessage()->set_continue_(true);

    return msg;
}

void HekateClientMessageBuilder::AddDebugResponseId(Proto::Client::HekateClientMessage &msg, const int debugId)
{
    msg.set_debug_response_id(debugId);
}

Proto::Client::HekateClientMessage HekateClientMessageBuilder::CreateReplayMessage(const Proto::Server::HekateMessage &serverMsg)
{
    Proto::Client::HekateClientMessage msg;
    msg.mutable_clientmessage()->set_type(Proto::Client::MessageType::REPLAY);
    msg.mutable_clientmessage()->mutable_replay()->set_allocated_message(const_cast<Proto::Server::HekateToClientMessage *>(&serverMsg.hook_info()));

    return msg;
}

}
}
}