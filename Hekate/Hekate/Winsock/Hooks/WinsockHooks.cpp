#include "WinsockHooks.h"

#include "App/IHekate.h"
#include "Protobuf/Message/HekateServerMessageBuilder.h"

namespace Hekate
{
namespace Winsock
{
namespace Hooks
{

WinsockHooks::WinsockHooks(Hekate::App::IHekate * const pApp) :
    m_pApp{ pApp },
    m_platformHook{ new Platform::WinsockPlatformHook(this) }
{
}

const Filter::FilteredInput WinsockHooks::CheckFilters(const char * const buffer, const int length)
{
    const auto filteredInput = m_pApp->ApplyFilter(buffer, length);
    if (filteredInput.hit)
    {
        auto filtermsg =
            Protobuf::Message::HekateServerMessageBuilder::CreateFilterHitMessage(filteredInput.id,
                filteredInput.breakOnHit);
        m_pApp->SendHekateMessage(filtermsg);

        if (filteredInput.breakOnHit)
        {
            m_pApp->Pause();
        }
    }

    return filteredInput;
}

int WinsockHooks::SendHook(SOCKET socket, const char *pBuffer, int iLength, int iFlags)
{
    sendFnc sendTrampoline = (sendFnc)m_pApp->GetCallAddress(WinsockFunction::send);

    auto msg = Protobuf::Message::HekateServerMessageBuilder::CreateSendMessage(socket, pBuffer,
        iLength, iFlags);
    m_pApp->SendHekateMessage(msg);

    auto filteredInput = CheckFilters(pBuffer, iLength);
    if (filteredInput.newData)
    {
        return sendTrampoline(socket, filteredInput.bytes.get(), filteredInput.length, iFlags);
    }
    return sendTrampoline(socket, pBuffer, iLength, iFlags);
}

int WinsockHooks::SendToHook(SOCKET socket, const char *pBuffer, int iLength, int iFlags,
    const struct sockaddr *pTo, int iToLength)
{
    sendtoFnc sendToTrampoline = (sendtoFnc)m_pApp->GetCallAddress(WinsockFunction::sendto);

    auto msg = Protobuf::Message::HekateServerMessageBuilder::CreateSendToMessage(socket, pBuffer,
        iLength, iFlags, pTo, iToLength);
    m_pApp->SendHekateMessage(msg);

    auto filteredInput = CheckFilters(pBuffer, iLength);

    if (filteredInput.newData)
    {
        return sendToTrampoline(socket, filteredInput.bytes.get(), filteredInput.length, iFlags, pTo, iToLength);
    }
    return sendToTrampoline(socket, pBuffer, iLength, iFlags, pTo, iToLength);
}

int WinsockHooks::WSASendHook(SOCKET socket, LPWSABUF lpBuffers, DWORD dwBufferCount,
    LPDWORD lpNumberOfBytesSent, DWORD dwFlags, LPWSAOVERLAPPED lpOverlapped,
    LPOVERLAPPED_COMPLETION_ROUTINE lpCompletionRoutine)
{
    WSASendFnc WSASendTrampoline = (WSASendFnc)m_pApp->GetCallAddress(WinsockFunction::WSASend);

    auto msg = Protobuf::Message::HekateServerMessageBuilder::CreateWSASendMessage(socket, lpBuffers,
        dwBufferCount, lpNumberOfBytesSent, dwFlags, lpOverlapped, lpCompletionRoutine);
    m_pApp->SendHekateMessage(msg);

    std::vector<Filter::FilteredInput> vecFiltered;
    for (DWORD i = 0; i < dwBufferCount; ++i)
    {
        auto filteredInput = CheckFilters(lpBuffers[i].buf, lpBuffers[i].len);
        if (filteredInput.newData)
        {
            lpBuffers[i].buf = filteredInput.bytes.get();
            lpBuffers[i].len = filteredInput.length;
            vecFiltered.emplace_back(std::move(filteredInput));
        }
    }

    return WSASendTrampoline(socket, lpBuffers, dwBufferCount, lpNumberOfBytesSent, dwFlags, lpOverlapped,
        lpCompletionRoutine);
}

int WinsockHooks::WSASendToHook(SOCKET socket, LPWSABUF lpBuffers, DWORD dwBufferCount,
    LPDWORD lpNumberOfBytesSent, DWORD dwFlags, const struct sockaddr *lpTo, int iToLength,
    LPWSAOVERLAPPED lpOverlapped, LPWSAOVERLAPPED_COMPLETION_ROUTINE lpCompletionRoutine)
{
    WSASendToFnc WSASendToTrampoline = (WSASendToFnc)m_pApp->GetCallAddress(WinsockFunction::WSASendTo);

    auto msg = Protobuf::Message::HekateServerMessageBuilder::CreateWSASendToMessage(socket, lpBuffers,
        dwBufferCount, lpNumberOfBytesSent, dwFlags, lpTo, iToLength, lpOverlapped,
        lpCompletionRoutine);
    m_pApp->SendHekateMessage(msg);

    std::vector<Filter::FilteredInput> vecFiltered;
    for (DWORD i = 0; i < dwBufferCount; ++i)
    {
        auto filteredInput = CheckFilters(lpBuffers[i].buf, lpBuffers[i].len);
        if (filteredInput.newData)
        {
            lpBuffers[i].buf = filteredInput.bytes.get();
            lpBuffers[i].len = filteredInput.length;
            vecFiltered.emplace_back(std::move(filteredInput));
        }
    }

    return WSASendToTrampoline(socket, lpBuffers, dwBufferCount, lpNumberOfBytesSent, dwFlags, lpTo,
        iToLength, lpOverlapped, lpCompletionRoutine);
}

int WinsockHooks::RecvHook(SOCKET socket, char *pBuffer, int iLength, int iFlags)
{
    recvFnc recvTrampoline = (recvFnc)m_pApp->GetCallAddress(WinsockFunction::recv);

    auto msg = Protobuf::Message::HekateServerMessageBuilder::CreateRecvMessage(socket, pBuffer, iLength,
        iFlags);
    m_pApp->SendHekateMessage(msg);

    auto filteredInput = CheckFilters(pBuffer, iLength);

    if (filteredInput.newData)
    {
        return recvTrampoline(socket, filteredInput.bytes.get(), filteredInput.length, iFlags);
    }
    return recvTrampoline(socket, pBuffer, iLength, iFlags);
}

int WinsockHooks::RecvFromHook(SOCKET socket, char *pBuffer, int iLength, int iFlags, struct sockaddr *pFrom, int *piFromLength)
{
    recvfromFnc recvfromTrampoline = (recvfromFnc)m_pApp->GetCallAddress(WinsockFunction::recvfrom);

    auto msg = Protobuf::Message::HekateServerMessageBuilder::CreateRecvFromMessage(socket, pBuffer,
        iLength, iFlags, pFrom, piFromLength);
    m_pApp->SendHekateMessage(msg);

    auto filteredInput = CheckFilters(pBuffer, iLength);

    if (filteredInput.newData)
    {
        return recvfromTrampoline(socket, filteredInput.bytes.get(), filteredInput.length, iFlags, pFrom, piFromLength);
    }
    return recvfromTrampoline(socket, pBuffer, iLength, iFlags, pFrom, piFromLength);
}

int WinsockHooks::WSARecvHook(SOCKET socket, LPWSABUF lpBuffers, DWORD dwBufferCount,
    LPDWORD lpNumberOfBytesRecvd, LPDWORD lpFlags, LPWSAOVERLAPPED lpOverlapped,
    LPWSAOVERLAPPED_COMPLETION_ROUTINE lpCompletionRoutine)
{
    WSARecvFnc WSARecvTrampoline = (WSARecvFnc)m_pApp->GetCallAddress(WinsockFunction::WSARecv);

    auto msg = Protobuf::Message::HekateServerMessageBuilder::CreateWSARecvMessage(socket, lpBuffers,
        dwBufferCount, lpNumberOfBytesRecvd, lpFlags, lpOverlapped, lpCompletionRoutine);
    m_pApp->SendHekateMessage(msg);

    std::vector<Filter::FilteredInput> vecFiltered;
    for (DWORD i = 0; i < dwBufferCount; ++i)
    {
        auto filteredInput = CheckFilters(lpBuffers[i].buf, lpBuffers[i].len);
        if (filteredInput.newData)
        {
            lpBuffers[i].buf = filteredInput.bytes.get();
            lpBuffers[i].len = filteredInput.length;
            vecFiltered.emplace_back(std::move(filteredInput));
        }
    }

    return WSARecvTrampoline(socket, lpBuffers, dwBufferCount, lpNumberOfBytesRecvd, lpFlags,
        lpOverlapped, lpCompletionRoutine);
}

int WinsockHooks::WSARecvFromHook(SOCKET socket, LPWSABUF lpBuffers, DWORD dwBufferCount,
    LPDWORD lpNumberOfBytesRecvd, LPDWORD lpFlags, struct sockaddr *lpFrom, LPINT lpFromLength,
    LPWSAOVERLAPPED lpOverlapped, LPWSAOVERLAPPED_COMPLETION_ROUTINE lpCompletionRoutine)
{
    WSARecvFromFnc WSARecvFromTrampoline = (WSARecvFromFnc)m_pApp->GetCallAddress(WinsockFunction::WSARecvFrom);

    auto msg = Protobuf::Message::HekateServerMessageBuilder::CreateWSARecvFromMessage(socket, lpBuffers,
        dwBufferCount, lpNumberOfBytesRecvd, lpFlags, lpFrom, lpFromLength, lpOverlapped,
        lpCompletionRoutine);
    m_pApp->SendHekateMessage(msg);

    std::vector<Filter::FilteredInput> vecFiltered;
    for (DWORD i = 0; i < dwBufferCount; ++i)
    {
        auto filteredInput = CheckFilters(lpBuffers[i].buf, lpBuffers[i].len);
        if (filteredInput.newData)
        {
            lpBuffers[i].buf = filteredInput.bytes.get();
            lpBuffers[i].len = filteredInput.length;
            vecFiltered.emplace_back(std::move(filteredInput));
        }
    }

    return WSARecvFromTrampoline(socket, lpBuffers, dwBufferCount, lpNumberOfBytesRecvd, lpFlags, lpFrom,
        lpFromLength, lpOverlapped, lpCompletionRoutine);
}

}
}
}
