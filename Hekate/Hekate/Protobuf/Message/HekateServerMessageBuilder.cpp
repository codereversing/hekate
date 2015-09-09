#include "Protobuf/Message/HekateServerMessageBuilder.h"

namespace Hekate
{
namespace Protobuf
{
namespace Message
{

Proto::Server::HekateMessage HekateServerMessageBuilder::CreateSendMessage(SOCKET socket, const char *buffer, int length, int flags)
{
    Proto::Server::HekateMessage msg;
    msg.mutable_hook_info()->mutable_send()->set_socket((google::protobuf::int64)socket);
    msg.mutable_hook_info()->mutable_send()->set_buffer(buffer, length);
    msg.mutable_hook_info()->mutable_send()->set_length(length);
    msg.mutable_hook_info()->mutable_send()->set_flags(flags);

    return msg;
}

Proto::Server::HekateMessage HekateServerMessageBuilder::CreateSendToMessage(SOCKET socket, const char *buffer, int length, int flags,
    const struct sockaddr *to, int tolength)
{
    Proto::Server::HekateMessage msg;
    msg.mutable_hook_info()->mutable_sendto()->set_socket((google::protobuf::int64)socket);
    msg.mutable_hook_info()->mutable_sendto()->set_buffer(buffer, length);
    msg.mutable_hook_info()->mutable_sendto()->set_length(length);
    msg.mutable_hook_info()->mutable_sendto()->set_flags(flags);
    msg.mutable_hook_info()->mutable_sendto()->set_to_address((google::protobuf::int64)to);
    msg.mutable_hook_info()->mutable_sendto()->set_to_length(tolength);

    return msg;
}

Proto::Server::HekateMessage HekateServerMessageBuilder::CreateWSASendMessage(SOCKET socket, LPWSABUF buffers, DWORD bufferCount,
    LPDWORD numberOfBytesSent, DWORD flags, LPWSAOVERLAPPED overlapped,
    LPOVERLAPPED_COMPLETION_ROUTINE completionRoutine)
{
    Proto::Server::HekateMessage msg;
    msg.mutable_hook_info()->mutable_wsasend()->set_socket((google::protobuf::int64)socket);
    for (DWORD i = 0; i < bufferCount; ++i)
    {
        msg.mutable_hook_info()->mutable_wsasend()->add_buffers(buffers[i].buf,
            buffers[i].len);
        msg.mutable_hook_info()->mutable_wsasend()->add_buffer_size(buffers[i].len);
    }
    msg.mutable_hook_info()->mutable_wsasend()->set_count(bufferCount);
    msg.mutable_hook_info()->mutable_wsasend()->set_bytes_sent_address((google::protobuf::int64)numberOfBytesSent);
    msg.mutable_hook_info()->mutable_wsasend()->set_flags(flags);
    msg.mutable_hook_info()->mutable_wsasend()->set_overlapped_address((google::protobuf::int64)overlapped);
    msg.mutable_hook_info()->mutable_wsasend()->set_overlapped_routine_address((google::protobuf::int64)completionRoutine);

    return msg;
}

Proto::Server::HekateMessage HekateServerMessageBuilder::CreateWSASendToMessage(SOCKET socket, LPWSABUF buffers, DWORD bufferCount,
    LPDWORD numberOfBytesSent, DWORD flags, const struct sockaddr *to, int toLength,
    LPWSAOVERLAPPED overlapped, LPWSAOVERLAPPED_COMPLETION_ROUTINE completionRoutine)
{
    Proto::Server::HekateMessage msg;
    msg.mutable_hook_info()->mutable_wsasendto()->set_socket((google::protobuf::int64)socket);
    for (DWORD i = 0; i < bufferCount; ++i)
    {
        msg.mutable_hook_info()->mutable_wsasendto()->add_buffers(buffers[i].buf,
            buffers[i].len);
        msg.mutable_hook_info()->mutable_wsasendto()->add_buffer_size(buffers[i].len);
    }
    msg.mutable_hook_info()->mutable_wsasendto()->set_count(bufferCount);
    msg.mutable_hook_info()->mutable_wsasendto()->set_bytes_sent_address((google::protobuf::int64)numberOfBytesSent);
    msg.mutable_hook_info()->mutable_wsasendto()->set_flags(flags);
    msg.mutable_hook_info()->mutable_wsasendto()->set_to_address((google::protobuf::int64)to);
    msg.mutable_hook_info()->mutable_wsasendto()->set_to_length(toLength);
    msg.mutable_hook_info()->mutable_wsasendto()->set_overlapped_address((google::protobuf::int64)overlapped);
    msg.mutable_hook_info()->mutable_wsasendto()->set_overlapped_routine_address((google::protobuf::int64)completionRoutine);

    return msg;
}

Proto::Server::HekateMessage HekateServerMessageBuilder::CreateRecvMessage(SOCKET socket, char *buffer, int length, int flags)
{
    Proto::Server::HekateMessage msg;
    msg.mutable_hook_info()->mutable_recv()->set_socket((google::protobuf::int64)socket);
    msg.mutable_hook_info()->mutable_recv()->set_buffer((size_t)buffer);
    msg.mutable_hook_info()->mutable_recv()->set_length(length);
    msg.mutable_hook_info()->mutable_recv()->set_flags(flags);

    return msg;
}

Proto::Server::HekateMessage HekateServerMessageBuilder::CreateRecvFromMessage(SOCKET socket, char *buffer, int length, int flags,
struct sockaddr *from, int *fromLength)
{
    Proto::Server::HekateMessage msg;
    msg.mutable_hook_info()->mutable_recvfrom()->set_socket((google::protobuf::int64)socket);
    msg.mutable_hook_info()->mutable_recvfrom()->set_buffer((size_t)buffer);
    msg.mutable_hook_info()->mutable_recvfrom()->set_length(length);
    msg.mutable_hook_info()->mutable_recvfrom()->set_flags(flags);
    msg.mutable_hook_info()->mutable_recvfrom()->set_from_address((google::protobuf::int64)from);
    msg.mutable_hook_info()->mutable_recvfrom()->set_from_length_address((google::protobuf::int64)fromLength);

    return msg;
}

Proto::Server::HekateMessage HekateServerMessageBuilder::CreateWSARecvMessage(SOCKET socket, LPWSABUF buffers, DWORD bufferCount,
    LPDWORD numberOfBytesRecvd, LPDWORD flags, LPWSAOVERLAPPED overlapped,
    LPWSAOVERLAPPED_COMPLETION_ROUTINE completionRoutine)
{
    Proto::Server::HekateMessage msg;
    msg.mutable_hook_info()->mutable_wsarecv()->set_socket((google::protobuf::int64)socket);
    for (DWORD i = 0; i < bufferCount; ++i)
    {
        msg.mutable_hook_info()->mutable_wsarecv()->add_buffers((size_t)buffers[i].buf);
        msg.mutable_hook_info()->mutable_wsarecv()->add_buffer_size(buffers[i].len);
    }
    msg.mutable_hook_info()->mutable_wsarecv()->set_count(bufferCount);
    msg.mutable_hook_info()->mutable_wsarecv()->set_bytes_received_address((google::protobuf::int64)numberOfBytesRecvd);
    msg.mutable_hook_info()->mutable_wsarecv()->set_flags_address((google::protobuf::int64)flags);
    msg.mutable_hook_info()->mutable_wsarecv()->set_overlapped_address((google::protobuf::int64)overlapped);
    msg.mutable_hook_info()->mutable_wsarecv()->set_overlapped_routine_address((google::protobuf::int64)completionRoutine);

    return msg;
}

Proto::Server::HekateMessage HekateServerMessageBuilder::CreateWSARecvFromMessage(SOCKET socket, LPWSABUF buffers, DWORD bufferCount,
    LPDWORD numberOfBytesRecvd, LPDWORD flags, struct sockaddr *from, LPINT fromLength,
    LPWSAOVERLAPPED overlapped, LPWSAOVERLAPPED_COMPLETION_ROUTINE completionRoutine)
{
    Proto::Server::HekateMessage msg;
    msg.mutable_hook_info()->mutable_wsarecvfrom()->set_socket((google::protobuf::int64)socket);
    for (DWORD i = 0; i < bufferCount; ++i)
    {
        msg.mutable_hook_info()->mutable_wsarecvfrom()->add_buffers((size_t)buffers[i].buf);
        msg.mutable_hook_info()->mutable_wsarecvfrom()->add_buffer_size(buffers[i].len);
    }
    msg.mutable_hook_info()->mutable_wsarecvfrom()->set_count(bufferCount);
    msg.mutable_hook_info()->mutable_wsarecvfrom()->set_bytes_received_address((google::protobuf::int64)numberOfBytesRecvd);
    msg.mutable_hook_info()->mutable_wsarecvfrom()->set_flags_address((google::protobuf::int64)flags);
    msg.mutable_hook_info()->mutable_wsarecvfrom()->set_from_address((google::protobuf::int64)from);
    msg.mutable_hook_info()->mutable_wsarecvfrom()->set_from_length_address((google::protobuf::int64)fromLength);
    msg.mutable_hook_info()->mutable_wsarecvfrom()->set_overlapped_address((google::protobuf::int64)overlapped);
    msg.mutable_hook_info()->mutable_wsarecvfrom()->set_overlapped_routine_address((google::protobuf::int64)completionRoutine);

    return msg;
}

Proto::Server::HekateMessage HekateServerMessageBuilder::CreateAcknowledgeMessage(const int acknowledgeCode)
{
    Proto::Server::HekateMessage msg;
    msg.mutable_acknowledge()->set_debug_response_id(acknowledgeCode);

    return msg;
}

Proto::Server::HekateMessage HekateServerMessageBuilder::CreateFilterHitMessage(const int filterId, const bool isBroken)
{
    Proto::Server::HekateMessage msg;
    msg.mutable_hook_info()->mutable_filter_message()->set_filter_id(filterId);
    msg.mutable_hook_info()->mutable_filter_message()->set_broken(isBroken);

    return msg;
}

}
}
}
