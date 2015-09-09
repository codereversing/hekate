#pragma once

#include <WinSock2.h>

namespace Hekate
{
namespace Winsock
{
namespace Hooks
{

class WinsockHookFunctions
{
public:
    
    WinsockHookFunctions() = default;
    virtual ~WinsockHookFunctions() = default;

    virtual int SendHook(SOCKET socket, const char *pBuffer, int iLength, int iFlags) = 0;
    virtual int SendToHook(SOCKET socket, const char *pBuffer, int iLength, int iFlags,
        const struct sockaddr *pTo, int iToLength) = 0;
    virtual int WSASendHook(SOCKET socket, LPWSABUF lpBuffers, DWORD dwBufferCount,
        LPDWORD lpNumberOfBytesSent, DWORD dwFlags, LPWSAOVERLAPPED lpOverlapped,
        LPOVERLAPPED_COMPLETION_ROUTINE lpCompletionRoutine) = 0;
    virtual int WSASendToHook(SOCKET socket, LPWSABUF lpBuffers, DWORD dwBufferCount,
        LPDWORD lpNumberOfBytesSent, DWORD dwFlags, const struct sockaddr *lpTo, int iToLength,
        LPWSAOVERLAPPED lpOverlapped, LPWSAOVERLAPPED_COMPLETION_ROUTINE lpCompletionRoutine) = 0;

    virtual int RecvHook(SOCKET socket, char *pBuffer, int iLength, int iFlags) = 0;
    virtual int RecvFromHook(SOCKET socket, char *pBuffer, int iLength, int iFlags,
        struct sockaddr *pFrom, int *piFromLength) = 0;
    virtual int WSARecvHook(SOCKET socket, LPWSABUF lpBuffers, DWORD dwBufferCount,
        LPDWORD lpNumberOfBytesRecvd, LPDWORD lpFlags, LPWSAOVERLAPPED lpOverlapped,
        LPWSAOVERLAPPED_COMPLETION_ROUTINE lpCompletionRoutine) = 0;
    virtual int WSARecvFromHook(SOCKET socket, LPWSABUF lpBuffers, DWORD dwBufferCount,
        LPDWORD lpNumberOfBytesRecvd, LPDWORD lpFlags, struct sockaddr *lpFrom, LPINT lpFromLength,
        LPWSAOVERLAPPED lpOverlapped, LPWSAOVERLAPPED_COMPLETION_ROUTINE lpCompletionRoutine) = 0;
};

}
}
}