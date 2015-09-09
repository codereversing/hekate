#pragma once

#include "Protobuf/Proto/Server/HekateServerProto.pb.h"

#include <WinSock2.h>

namespace Hekate
{
namespace Protobuf
{
namespace Message
{

class HekateServerMessageBuilder final
{
public:
    HekateServerMessageBuilder() = delete;
    ~HekateServerMessageBuilder() = delete;

    static Proto::Server::HekateMessage CreateSendMessage(SOCKET socket, const char *buffer, int length, int flags);

    static Proto::Server::HekateMessage CreateSendToMessage(SOCKET socket, const char *buffer, int length, int flags,
        const struct sockaddr *to, int tolength);

    static Proto::Server::HekateMessage CreateWSASendMessage(SOCKET socket, LPWSABUF buffers, DWORD bufferCount,
        LPDWORD numberOfBytesSent, DWORD flags, LPWSAOVERLAPPED overlapped,
        LPOVERLAPPED_COMPLETION_ROUTINE completionRoutine);

    static Proto::Server::HekateMessage CreateWSASendToMessage(SOCKET socket, LPWSABUF buffers, DWORD bufferCount,
        LPDWORD numberOfBytesSent, DWORD flags, const struct sockaddr *to, int toLength,
        LPWSAOVERLAPPED overlapped, LPWSAOVERLAPPED_COMPLETION_ROUTINE completionRoutine);

    static Proto::Server::HekateMessage CreateRecvMessage(SOCKET socket, char *buffer, int length, int flags);

    static Proto::Server::HekateMessage CreateRecvFromMessage(SOCKET socket, char *buffer, int length, int flags,
    struct sockaddr *from, int *fromLength);

    static Proto::Server::HekateMessage CreateWSARecvMessage(SOCKET socket, LPWSABUF buffers, DWORD bufferCount,
        LPDWORD numberOfBytesRecvd, LPDWORD flags, LPWSAOVERLAPPED overlapped,
        LPWSAOVERLAPPED_COMPLETION_ROUTINE completionRoutine);

    static Proto::Server::HekateMessage CreateWSARecvFromMessage(SOCKET socket, LPWSABUF buffers, DWORD bufferCount,
        LPDWORD numberOfBytesRecvd, LPDWORD flags, struct sockaddr *from, LPINT fromLength,
        LPWSAOVERLAPPED overlapped, LPWSAOVERLAPPED_COMPLETION_ROUTINE completionRoutine);

    static Proto::Server::HekateMessage CreateAcknowledgeMessage(const int acknowledgeCode);

    static Proto::Server::HekateMessage CreateFilterHitMessage(const int filterId, const bool isBroken);

};

}
}
}