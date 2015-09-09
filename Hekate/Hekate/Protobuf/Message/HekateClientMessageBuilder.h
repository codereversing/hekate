#pragma once

#include "Protobuf/Proto/Client/HekateClientProto.pb.h"
#include "Protobuf/Proto/Server/HekateServerProto.pb.h"

#include <string>

namespace Hekate
{
namespace Protobuf
{
namespace Message
{

class HekateClientMessageBuilder
{
public:
    HekateClientMessageBuilder() = delete;
    ~HekateClientMessageBuilder() = delete;

    static Proto::Client::HekateClientMessage CreateAddHookMessage(const std::string &name);

    static Proto::Client::HekateClientMessage CreateRemoveHookMessage(const std::string &name);

    static Proto::Client::HekateClientMessage CreateByteFilterMessage(const int filterId, const bool breakOnHit,
        const char * const byteValues, const int * const bytesIndices, const int byteCount);

    static Proto::Client::HekateClientMessage CreateLengthFilterMessage(const int filterId, const bool breakOnHit,
        const int lengthValue, Proto::Client::LengthType type);

    static Proto::Client::HekateClientMessage CreateSubstringFilterMessage(const int filterId, const bool breakOnHit,
        const char * const match, const int length);

    static void AddSubstituteMessage(Proto::Client::HekateClientMessage &msg, const char * const replacementBytes,
        const int * const replacementIndices, const int byteCount);

    static void AddReplacementMessage(Proto::Client::HekateClientMessage &msg, const char * const buffer,
        const int length);

    static void AddDebugResponseId(Proto::Client::HekateClientMessage &msg, const int debugId);

    static Proto::Client::HekateClientMessage CreateRemoveFilterMessage(const int filterId);

    static Proto::Client::HekateClientMessage CreateContinueMessage();

    static Proto::Client::HekateClientMessage CreateReplayMessage(const Proto::Server::HekateMessage &serverMsg);
};

}
}
}