#pragma once

#pragma comment(lib, "Ws2_32.lib")

#include <string>

#include <WinSock2.h>

namespace Hekate
{
namespace Winsock
{

using sendFnc = int (WINAPI *)(SOCKET socket, const char *pBuffer, int iLength, int iFlags);
using sendtoFnc = int (WINAPI *)(SOCKET socket, const char *pBuffer, int iLength, int iFlags,
    const struct sockaddr *pTo, int iToLength);
using WSASendFnc = int (WINAPI *)(SOCKET socket, LPWSABUF lpBuffers, DWORD dwBufferCount,
    LPDWORD lpNumberOfBytesSent, DWORD dwFlags, LPWSAOVERLAPPED lpOverlapped,
    LPOVERLAPPED_COMPLETION_ROUTINE lpCompletionRoutine);
using WSASendToFnc = int (WINAPI *)(SOCKET socket, LPWSABUF lpBuffers, DWORD dwBufferCount,
    LPDWORD lpNumberOfBytesSent, DWORD dwFlags, const struct sockaddr *lpTo, int iToLength,
    LPWSAOVERLAPPED lpOverlapped, LPWSAOVERLAPPED_COMPLETION_ROUTINE lpCompletionRoutine);

using recvFnc = int (WINAPI *)(SOCKET socket, char *pBuffer, int iLength, int iFlags);
using recvfromFnc = int (WINAPI *)(SOCKET socket, char *pBuffer, int iLength, int iFlags,
    struct sockaddr *pFrom, int *piFromLength);
using WSARecvFnc = int (WINAPI *)(SOCKET socket, LPWSABUF lpBuffers, DWORD dwBufferCount,
    LPDWORD lpNumberOfBytesRecvd, LPDWORD lpFlags, LPWSAOVERLAPPED lpOverlapped,
    LPWSAOVERLAPPED_COMPLETION_ROUTINE lpCompletionRoutine);
using WSARecvFromFnc = int (WINAPI *)(SOCKET socket, LPWSABUF lpBuffers, DWORD dwBufferCount,
    LPDWORD lpNumberOfBytesRecvd, LPDWORD lpFlags, struct sockaddr *lpFrom, LPINT lpFromLength,
    LPWSAOVERLAPPED lpOverlapped, LPWSAOVERLAPPED_COMPLETION_ROUTINE lpCompletionRoutine);

extern sendFnc sendAddr;
extern sendtoFnc sendtoAddr;
extern WSASendFnc WSASendAddr;
extern WSASendToFnc WSASendToAddr;

extern recvFnc recvAddr;
extern recvfromFnc recvfromAddr;
extern WSARecvFnc WSARecvAddr;
extern WSARecvFromFnc WSARecvFromAddr;

enum class WinsockFunction
{
    send = 0,
    sendto,
    WSASend,
    WSASendTo,

    recv,
    recvfrom,
    WSARecv,
    WSARecvFrom,

    Unknown
};

std::string WinsockFunctionToString(const WinsockFunction function);
const WinsockFunction WinsockFunctionFromString(const std::string &name);

}
}
