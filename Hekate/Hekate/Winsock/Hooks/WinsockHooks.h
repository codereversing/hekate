#pragma once

#include "Filter/HekateFilter.h"
#include "Winsock/Hooks/WinsockHookFunctions.h"
#include "Winsock/Platform/WinsockPlatformHook.h"

#include <memory>

namespace Hekate
{
namespace App
{
    class IHekate;
}
}

namespace Hekate
{
namespace Winsock
{
namespace Hooks
{

class WinsockHooks final : public WinsockHookFunctions
{
public:
    friend class Platform::WinsockPlatformHook;

    WinsockHooks() = delete;

    WinsockHooks(App::IHekate * const pApp);
    ~WinsockHooks() = default;

private:
    int SendHook(SOCKET socket, const char *pBuffer, int iLength, int iFlags) override;
    int SendToHook(SOCKET socket, const char *pBuffer, int iLength, int iFlags,
        const struct sockaddr *pTo, int iToLength) override;
    int WSASendHook(SOCKET socket, LPWSABUF lpBuffers, DWORD dwBufferCount,
        LPDWORD lpNumberOfBytesSent, DWORD dwFlags, LPWSAOVERLAPPED lpOverlapped,
        LPOVERLAPPED_COMPLETION_ROUTINE lpCompletionRoutine) override;
    int WSASendToHook(SOCKET socket, LPWSABUF lpBuffers, DWORD dwBufferCount,
        LPDWORD lpNumberOfBytesSent, DWORD dwFlags, const struct sockaddr *lpTo, int iToLength,
        LPWSAOVERLAPPED lpOverlapped, LPWSAOVERLAPPED_COMPLETION_ROUTINE lpCompletionRoutine) override;

    int RecvHook(SOCKET socket, char *pBuffer, int iLength, int iFlags) override;
    int RecvFromHook(SOCKET socket, char *pBuffer, int iLength, int iFlags,
        struct sockaddr *pFrom, int *piFromLength) override;
    int WSARecvHook(SOCKET socket, LPWSABUF lpBuffers, DWORD dwBufferCount,
        LPDWORD lpNumberOfBytesRecvd, LPDWORD lpFlags, LPWSAOVERLAPPED lpOverlapped,
        LPWSAOVERLAPPED_COMPLETION_ROUTINE lpCompletionRoutine) override;
    int WSARecvFromHook(SOCKET socket, LPWSABUF lpBuffers, DWORD dwBufferCount,
        LPDWORD lpNumberOfBytesRecvd, LPDWORD lpFlags, struct sockaddr *lpFrom, LPINT lpFromLength,
        LPWSAOVERLAPPED lpOverlapped, LPWSAOVERLAPPED_COMPLETION_ROUTINE lpCompletionRoutine) override;

    const Filter::FilteredInput CheckFilters(const char * const buffer, const int length);

    App::IHekate * const m_pApp;
    std::unique_ptr<Platform::WinsockPlatformHook> m_platformHook;
};

}
}
}
