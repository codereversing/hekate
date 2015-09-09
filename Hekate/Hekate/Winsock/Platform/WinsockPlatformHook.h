#pragma once

#include "Winsock/WinsockFunction.h"

#include <map>
#include <Winsock2.h>

namespace Hekate
{
namespace Winsock
{
namespace Hooks
{
class WinsockHookFunctions;
}
}
}

namespace Hekate
{
namespace Winsock
{
namespace Platform
{

class WinsockPlatformHook
{
public:
    WinsockPlatformHook(Hooks::WinsockHookFunctions *pHooks);
    ~WinsockPlatformHook() = default;

    static const DWORD_PTR GetHookAddress(const WinsockFunction &function);

private:

    static int WINAPI SendGateway(SOCKET socket, const char *pBuffer, int iLength, int iFlags);
    static int WINAPI SendToGateway(SOCKET socket, const char *pBuffer, int iLength, int iFlags,
        const struct sockaddr *pTo, int iToLength);
    static int WINAPI WSASendGateway(SOCKET socket, LPWSABUF lpBuffers, DWORD dwBufferCount,
        LPDWORD lpNumberOfBytesSent, DWORD dwFlags, LPWSAOVERLAPPED lpOverlapped,
        LPOVERLAPPED_COMPLETION_ROUTINE lpCompletionRoutine);
    static int WINAPI WSASendToGateway(SOCKET socket, LPWSABUF lpBuffers, DWORD dwBufferCount,
        LPDWORD lpNumberOfBytesSent, DWORD dwFlags, const struct sockaddr *lpTo, int iToLength,
        LPWSAOVERLAPPED lpOverlapped, LPWSAOVERLAPPED_COMPLETION_ROUTINE lpCompletionRoutine);

    static int WINAPI RecvGateway(SOCKET socket, char *pBuffer, int iLength, int iFlags);
    static int WINAPI RecvFromGateway(SOCKET socket, char *pBuffer, int iLength, int iFlags,
        struct sockaddr *pFrom, int *piFromLength);
    static int WINAPI WSARecvGateway(SOCKET socket, LPWSABUF lpBuffers, DWORD dwBufferCount,
        LPDWORD lpNumberOfBytesRecvd, LPDWORD lpFlags, LPWSAOVERLAPPED lpOverlapped,
        LPWSAOVERLAPPED_COMPLETION_ROUTINE lpCompletionRoutine);
    static int WINAPI WSARecvFromGateway(SOCKET socket, LPWSABUF lpBuffers, DWORD dwBufferCount,
        LPDWORD lpNumberOfBytesRecvd, LPDWORD lpFlags, struct sockaddr *lpFrom, LPINT lpFromLength,
        LPWSAOVERLAPPED lpOverlapped, LPWSAOVERLAPPED_COMPLETION_ROUTINE lpCompletionRoutine);

    static std::map<WinsockFunction, DWORD_PTR> m_hookMap;
    static Hooks::WinsockHookFunctions *m_pHooks;
};

}
}
}