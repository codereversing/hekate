#include "Winsock/Platform/WinsockPlatformHook.h"

#include "App/HekateMain.h"
#include "Protobuf/Message/HekateServerMessageBuilder.h"
#include "Winsock/Hooks/WinsockHooks.h"
#include "Winsock/Hooks/WinsockHookFunctions.h"

#include "plog/Log.h"

namespace Hekate
{
namespace Winsock
{
namespace Platform
{

std::map<WinsockFunction, DWORD_PTR> WinsockPlatformHook::m_hookMap = 
{
    {WinsockFunction::send, (DWORD_PTR)SendGateway},
    {WinsockFunction::send, (DWORD_PTR)SendGateway},
    {WinsockFunction::sendto, (DWORD_PTR)SendToGateway},
    {WinsockFunction::WSASend, (DWORD_PTR)WSASendGateway},
    {WinsockFunction::WSASendTo, (DWORD_PTR)WSASendToGateway},
    {WinsockFunction::recv, (DWORD_PTR)RecvGateway},
    {WinsockFunction::recvfrom, (DWORD_PTR)RecvFromGateway},
    {WinsockFunction::WSARecv, (DWORD_PTR)WSARecvGateway},
    {WinsockFunction::WSARecvFrom, (DWORD_PTR)WSARecvFromGateway}
};
Hooks::WinsockHookFunctions *WinsockPlatformHook::m_pHooks;

WinsockPlatformHook::WinsockPlatformHook(Hooks::WinsockHookFunctions * const pHooks)
{
    m_pHooks = pHooks;
}

const DWORD_PTR WinsockPlatformHook::GetHookAddress(const WinsockFunction &function)
{
    return m_hookMap.find(function)->second;
}

int WINAPI WinsockPlatformHook::SendGateway(SOCKET socket, const char *pBuffer, int iLength, int iFlags)
{
    LOG_VERBOSE << "send hook hit\n"
        << "Dumping parameters\n"
        << "   - Socket: 0x" << std::hex << socket << '\n'
        << "   - Buffer address: 0x" << std::hex << (void *)pBuffer << '\n'
        << "   - Length: 0x" << std::hex << iLength << '\n'
        << "   - Flags: 0x" << std::hex << iFlags;

    return m_pHooks->SendHook(socket, pBuffer, iLength, iFlags);
}

int WINAPI WinsockPlatformHook::SendToGateway(SOCKET socket, const char *pBuffer, int iLength, int iFlags,
    const struct sockaddr *pTo, int iToLength)
{
    LOG_VERBOSE << "sendto hook hit\n"
        << "Dumping parameters\n"
        << "   - Socket: 0x" << std::hex << socket << '\n'
        << "   - Buffer address: 0x" << std::hex << (void *)pBuffer << '\n'
        << "   - Length: 0x" << std::hex << iLength << '\n'
        << "   - Flags: 0x" << std::hex << iFlags << '\n'
        << "   - Target socket address: 0x" << std::hex << pTo << '\n'
        << "   - Target socket address length: 0x" << std::hex << iToLength;

    return m_pHooks->SendToHook(socket, pBuffer, iLength, iFlags,
        pTo, iToLength);
}

int WINAPI WinsockPlatformHook::WSASendGateway(SOCKET socket, LPWSABUF lpBuffers, DWORD dwBufferCount,
    LPDWORD lpNumberOfBytesSent, DWORD dwFlags, LPWSAOVERLAPPED lpOverlapped,
    LPOVERLAPPED_COMPLETION_ROUTINE lpCompletionRoutine)
{
    LOG_VERBOSE << "WSASend hook hit\n"
        << "Dumping parameters\n"
        << "   - Socket 0x" << std::hex << socket << '\n'
        << "   - Buffers address: 0x" << std::hex << lpBuffers << '\n'
        << "   - Buffer count: 0x" << dwBufferCount << '\n'
        << "   - Num bytes sent address: 0x" << std::hex << lpNumberOfBytesSent << '\n'
        << "   - Flags: 0x" << std::hex << dwFlags << '\n'
        << "   - Overlapped address: 0x" << std::hex << lpOverlapped << '\n'
        << "   - Competion routine address: 0x" << std::hex << lpCompletionRoutine;

    return m_pHooks->WSASendHook(socket, lpBuffers, dwBufferCount,
        lpNumberOfBytesSent, dwFlags, lpOverlapped, lpCompletionRoutine);

}

int WINAPI WinsockPlatformHook::WSASendToGateway(SOCKET socket, LPWSABUF lpBuffers, DWORD dwBufferCount,
    LPDWORD lpNumberOfBytesSent, DWORD dwFlags, const struct sockaddr *lpTo, int iToLength,
    LPWSAOVERLAPPED lpOverlapped, LPWSAOVERLAPPED_COMPLETION_ROUTINE lpCompletionRoutine)
{
    LOG_VERBOSE << "WSASendTo hook hit\n"
        << "Dumping parameters\n"
        << "   - Socket 0x" << std::hex << socket << '\n'
        << "   - Buffers address: 0x" << std::hex << lpBuffers << '\n'
        << "   - Buffer count: 0x" << dwBufferCount << '\n'
        << "   - Num bytes sent address: 0x" << std::hex << lpNumberOfBytesSent << '\n'
        << "   - Flags: 0x" << std::hex << dwFlags << '\n'
        << "   - Target socket address: 0x" << std::hex << lpTo << '\n'
        << "   - Target socket address length: 0x" << std::hex << iToLength << '\n'
        << "   - Overlapped address: 0x" << std::hex << lpOverlapped << '\n'
        << "   - Competion routine address: 0x" << std::hex << lpCompletionRoutine;

    return m_pHooks->WSASendToHook(socket, lpBuffers, dwBufferCount,
        lpNumberOfBytesSent, dwFlags, lpTo, iToLength, lpOverlapped, lpCompletionRoutine);
}

int WINAPI WinsockPlatformHook::RecvGateway(SOCKET socket, char *pBuffer, int iLength, int iFlags)
{
    LOG_VERBOSE << "recv hook hit\n"
        << "Dumping parameters\n"
        << "   - Socket: 0x" << std::hex << socket << '\n'
        << "   - Buffer address: 0x" << std::hex << (void *)pBuffer << '\n'
        << "   - Length: 0x" << std::hex << iLength << '\n'
        << "   - Flags: 0x" << std::hex << iFlags;

    return m_pHooks->RecvHook(socket, pBuffer, iLength, iFlags);
}

int WINAPI WinsockPlatformHook::RecvFromGateway(SOCKET socket, char *pBuffer, int iLength, int iFlags,
    struct sockaddr *pFrom, int *piFromLength)
{
    LOG_VERBOSE << "recvfrom hook hit\n"
        << "Dumping parameters\n"
        << "   - Socket: 0x" << std::hex << socket << '\n'
        << "   - Buffer address: 0x" << std::hex << (void *)pBuffer << '\n'
        << "   - Length: 0x" << std::hex << iLength << '\n'
        << "   - Flags: 0x" << std::hex << iFlags << '\n'
        << "   - Source address pointer: 0x" << std::hex << pFrom << '\n'
        << "   - Source address pointer to size: 0x" << std::hex << piFromLength;

    return m_pHooks->RecvFromHook(socket, pBuffer, iLength, iFlags,
        pFrom, piFromLength);
}

int WINAPI WinsockPlatformHook::WSARecvGateway(SOCKET socket, LPWSABUF lpBuffers, DWORD dwBufferCount,
    LPDWORD lpNumberOfBytesRecvd, LPDWORD lpFlags, LPWSAOVERLAPPED lpOverlapped,
    LPWSAOVERLAPPED_COMPLETION_ROUTINE lpCompletionRoutine)
{
    LOG_VERBOSE << "WSARecvFrom hook hit\n"
        << "Dumping parameters\n"
        << "   - Socket 0x" << std::hex << socket << '\n'
        << "   - Buffers address: 0x" << std::hex << lpBuffers << '\n'
        << "   - Buffer count: 0x" << dwBufferCount << '\n'
        << "   - Num bytes recevied address: 0x" << std::hex << lpNumberOfBytesRecvd << '\n'
        << "   - Pointer to flags: 0x" << std::hex << lpFlags << '\n'
        << "   - Overlapped address: 0x" << std::hex << lpOverlapped << '\n'
        << "   - Competion routine address: 0x" << std::hex << lpCompletionRoutine;

    return m_pHooks->WSARecvHook(socket, lpBuffers, dwBufferCount,
        lpNumberOfBytesRecvd, lpFlags, lpOverlapped, lpCompletionRoutine);
}

int WINAPI WinsockPlatformHook::WSARecvFromGateway(SOCKET socket, LPWSABUF lpBuffers, DWORD dwBufferCount,
    LPDWORD lpNumberOfBytesRecvd, LPDWORD lpFlags, struct sockaddr *lpFrom, LPINT lpFromLength,
    LPWSAOVERLAPPED lpOverlapped, LPWSAOVERLAPPED_COMPLETION_ROUTINE lpCompletionRoutine)
{
    LOG_VERBOSE << "WSARecvFrom hook hit\n"
        << "Dumping parameters\n"
        << "   - Socket 0x" << std::hex << socket << '\n'
        << "   - Buffers address: 0x" << std::hex << lpBuffers << '\n'
        << "   - Buffer count: 0x" << dwBufferCount << '\n'
        << "   - Num bytes received address: 0x" << std::hex << lpNumberOfBytesRecvd << '\n'
        << "   - Pointer to flags: 0x" << std::hex << lpFlags << '\n'
        << "   - Source address pointer: 0x" << std::hex << lpFrom << '\n'
        << "   - Source address pointer to size: 0x" << std::hex << lpFromLength << '\n'
        << "   - Overlapped address: 0x" << std::hex << lpOverlapped << '\n'
        << "   - Competion routine address: 0x" << std::hex << lpCompletionRoutine;

    return m_pHooks->WSARecvFromHook(socket, lpBuffers, dwBufferCount,
        lpNumberOfBytesRecvd, lpFlags, lpFrom, lpFromLength,
        lpOverlapped, lpCompletionRoutine);
}

}
}
}