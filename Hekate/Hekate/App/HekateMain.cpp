#include "App/HekateMain.h"

#include "App/HekateMessageHandler.h"
#include "Common/Common.h"
#include "Filter/ByteFilter.h"
#include "Filter/LengthFilter.h"
#include "Filter/SubstringFilter.h"
#include "Hook/InlineHook.h"
#include "IPC/IPCNamedPipe.h"
#include "Protobuf/Message/HekateServerMessageBuilder.h"
#include "Winsock/Platform/WinsockPlatformHook.h"
#include "Winsock/WinsockFunction.h"
#include "Winsock/WinsockFunction.h"

#include "plog/Log.h"

namespace Hekate
{

namespace App
{

HekateMain::HekateMain()
{
    InitializeAddressMap();
    InitializeHookEngine();
    InitializeIPC();
    InitializeMessageHandler();
    InitializeFilter();
    InitializeWinsockHooks();
    InitializeWaitEvent();
}

void HekateMain::InitializeAddressMap()
{
    HMODULE hModule = GetModuleHandle(L"Ws2_32.dll");
    if (hModule == nullptr)
    {
        LOG_WARNING << "Ws2_32.dll was not loaded. Attempting to load manually...";
        hModule = LoadLibrary(L"Ws2_32.dll");
        if (hModule == nullptr)
        {
            LOG_FATAL << "Could not load Ws2_32.dll. Cannot get addresses of Winsock "
                << "functions to hook. Exiting.";
            exit(-1);
        }
    }

    bool bLoaded = Hekate::Common::ResolveImport(hModule, Winsock::sendAddr, "send");
    bLoaded &= Hekate::Common::ResolveImport(hModule, Winsock::sendtoAddr, "sendto");
    bLoaded &= Hekate::Common::ResolveImport(hModule, Winsock::WSASendAddr, "WSASend");
    bLoaded &= Hekate::Common::ResolveImport(hModule, Winsock::WSASendToAddr, "WSASendTo");

    bLoaded &= Hekate::Common::ResolveImport(hModule, Winsock::recvAddr, "recv");
    bLoaded &= Hekate::Common::ResolveImport(hModule, Winsock::recvfromAddr, "recvfrom");
    bLoaded &= Hekate::Common::ResolveImport(hModule, Winsock::WSARecvAddr, "WSARecv");
    bLoaded &= Hekate::Common::ResolveImport(hModule, Winsock::WSARecvFromAddr, "WSARecvFrom");

    if (!bLoaded)
    {
        LOG_FATAL << "Did not successfully resolve all Winsock functions. "
            << "Hooking engine will not install hooks until this is resolved. "
            << "Exiting.";
        exit(-1);
    }

    m_mapAddress.clear();

    m_mapAddress.emplace(std::make_pair(Winsock::WinsockFunction::send, (DWORD_PTR)Winsock::sendAddr));
    m_mapAddress.emplace(std::make_pair(Winsock::WinsockFunction::sendto, (DWORD_PTR)Winsock::sendtoAddr));
    m_mapAddress.emplace(std::make_pair(Winsock::WinsockFunction::WSASend, (DWORD_PTR)Winsock::WSASendAddr));
    m_mapAddress.emplace(std::make_pair(Winsock::WinsockFunction::WSASendTo, (DWORD_PTR)Winsock::WSASendToAddr));

    m_mapAddress.emplace(std::make_pair(Winsock::WinsockFunction::recv, (DWORD_PTR)Winsock::recvAddr));
    m_mapAddress.emplace(std::make_pair(Winsock::WinsockFunction::recvfrom, (DWORD_PTR)Winsock::recvfromAddr));
    m_mapAddress.emplace(std::make_pair(Winsock::WinsockFunction::WSARecv, (DWORD_PTR)Winsock::WSARecvAddr));
    m_mapAddress.emplace(std::make_pair(Winsock::WinsockFunction::WSARecvFrom, (DWORD_PTR)Winsock::WSARecvFromAddr));
}

void HekateMain::InitializeHookEngine()
{
    m_hookEngine = std::unique_ptr<Hook::HookEngine>(new Hekate::Hook::HookEngine);
}

void HekateMain::InitializeIPC()
{
    m_ipc = std::unique_ptr<IPC::IPCBase>(new Hekate::IPC::IPCNamedPipe(
        "\\\\.\\pipe\\HekatePipeOutbound",
        "\\\\.\\pipe\\HekatePipeInbound",
        [this](const std::unique_ptr<char[]> &bytes, const DWORD size)
    {
        Protobuf::Proto::Client::HekateClientMessage msg;
        auto success = msg.ParseFromArray(bytes.get(), size);
        if (!success)
        {
            LOG_ERROR << "Could not deserialize received message.";
        }
        else
        {
            RecvCallback(msg);
        }
    }));

    if (!m_ipc->Open())
    {
        LOG_FATAL << "Could not open IPC connections. Exiting.";
        exit(-1);
    }
}

void HekateMain::InitializeMessageHandler()
{
    m_msgHandler =
        std::unique_ptr<Protobuf::Message::IMessageHandler>(new App::HekateMessageHandler(this));
}

void HekateMain::InitializeFilter()
{
    m_filters = std::unique_ptr<Filter::HekateFilter>(new Filter::HekateFilter);
}

void HekateMain::InitializeWinsockHooks()
{
    m_winsockHooks = std::unique_ptr<Winsock::Hooks::WinsockHooks>
        (new Winsock::Hooks::WinsockHooks(this));
}

void HekateMain::InitializeWaitEvent()
{
    m_waitEvent = CreateEvent(nullptr, false, false, L"Hekate wait event");
}

const DWORD_PTR HekateMain::GetCallAddress(const Winsock::WinsockFunction function)
{
    const auto pHook = m_hookEngine->Find(m_mapAddress[function]);
    if (pHook == nullptr)
    {
        LOG_ERROR << "Could not get trampoline address since no hook exists at 0x"
            << std::hex << m_mapAddress[function];
        return 0;
    }

    return ((Hook::InlineHook *)pHook->get())->TrampolineAddress();
}

const Filter::FilteredInput HekateMain::ApplyFilter(const char * const bytes, const int length)
{
    Filter::FilteredInput filteredInput;

    m_filters->ParseInput(bytes, length, filteredInput);

    return filteredInput;
}

void HekateMain::RecvCallback(const Protobuf::Proto::Client::HekateClientMessage &msg)
{
    LOG_VERBOSE << "Successfully received message from client.";

    if (msg.has_debug_response_id())
    {
        LOG_VERBOSE << "Acknowledging message received";

        auto response =
            Protobuf::Message::HekateServerMessageBuilder::CreateAcknowledgeMessage(msg.debug_response_id());
        (void)SendHekateMessage(response);
    }

    if (msg.has_clientmessage())
    {
        (void)m_msgHandler->Parse(msg);
    }
    else
    {
        LOG_ERROR << "Client message was missing from received message.";
    }
}

const bool HekateMain::SendHekateMessage(const Protobuf::Proto::Server::HekateMessage &msg)
{
    if (msg.ByteSize() > 0)
    {
        std::unique_ptr<char[]> pBytes(new char[msg.ByteSize()]);
        bool bSuccess = msg.SerializeToArray(pBytes.get(), msg.ByteSize());
        if (bSuccess)
        {
            LOG_VERBOSE << "Successfully serialized Hekate message to client.";

            bSuccess = m_ipc->Send(pBytes.get(), msg.ByteSize());
            if (bSuccess)
            {
                LOG_VERBOSE << "Successfully sent out message to client.";
            }
        }
    }
    else
    {
        LOG_ERROR << "Could not send out message because byte size was zero.";
        return false;
    }

    return true;
}

const bool HekateMain::AddHook(const std::string &functionName)
{
    LOG_DEBUG << "Adding hook on " << functionName.c_str();

    auto function = Winsock::WinsockFunctionFromString(functionName);

    const bool bSuccess = m_hookEngine->Add(m_mapAddress[function],
        Winsock::Platform::WinsockPlatformHook::GetHookAddress(function));
    if (!bSuccess)
    {
        LOG_ERROR << "Could not install hook on "
            << Winsock::WinsockFunctionToString(function).c_str();
        return false;
    }

    return true;
}

const bool HekateMain::RemoveHook(const std::string &functionName)
{
    LOG_DEBUG << "Removing hook on " << functionName.c_str();

    auto function = Winsock::WinsockFunctionFromString(functionName);

    const bool bSuccess = m_hookEngine->Remove(m_mapAddress[function]);
    if (!bSuccess)
    {
        LOG_ERROR << "Could not remove hook on "
            << Winsock::WinsockFunctionToString(function).c_str();
        return false;
    }

    return true;
}

const bool HekateMain::AddFilter(const Protobuf::Proto::Client::AddFilterMessage &msg)
{
    if (msg.type() == Protobuf::Proto::Client::FilterType::BYTE)
    {
        LOG_DEBUG << "Adding byte filter with id = 0x" <<
            std::hex << msg.filter_id();

        m_filters->AddFilter(new Filter::ByteFilter(msg));
        return true;
    }
    if (msg.type() == Protobuf::Proto::Client::FilterType::LENGTH)
    {
        LOG_DEBUG << "Adding length filter with id = 0x" <<
            std::hex << msg.filter_id();

        m_filters->AddFilter(new Filter::LengthFilter(msg));
        return true;
    }
    if (msg.type() == Protobuf::Proto::Client::FilterType::SUBSTRING)
    {
        LOG_DEBUG << "Adding substring filter with id = 0x" <<
            std::hex << msg.filter_id();
        m_filters->AddFilter(new Filter::SubstringFilter(msg));
        return true;
    }

    LOG_ERROR << "Encountered unrecognized filter type.";

    return false;
}

const bool HekateMain::RemoveFilter(const int filterId)
{
    LOG_DEBUG << "Removing filter with id = 0x" <<
        std::hex << filterId;

    return m_filters->RemoveFilter(filterId);;
}

const bool HekateMain::Continue()
{
    LOG_DEBUG << "Attempting to resume socket activity.";

    const bool success = BOOLIFY(SetEvent(m_waitEvent()));
    if (!success)
    {
        LOG_ERROR << "Could not resume socket activity. Error = 0x"
            << std::hex << GetLastError();
    }

    return success;
}

const bool HekateMain::Pause()
{
    LOG_DEBUG << "Attempting to pause socket activity.";

    DWORD ret = WaitForSingleObject(m_waitEvent(), INFINITE);
    switch (ret)
    {
    case WAIT_ABANDONED:
        LOG_ERROR << "Could not pause sockets. Error = WAIT_ABANDONED.";
        break;
    case WAIT_TIMEOUT:
        LOG_ERROR << "Could not pause sockets. Timeout was hit (should be infinite).";
        break;
    case WAIT_FAILED:
        LOG_ERROR << "Could not pause socckets. Error = 0x" <<
            std::hex << GetLastError();
        break;
    }

    return (ret == WAIT_OBJECT_0);
}

const bool HekateMain::Replay(const Protobuf::Proto::Client::ReplayMessage &msg)
{
    auto wsabufDeleter = [](WSABUF * buffers) { delete [] buffers; };
    int bytesSent = 0;

    if (msg.message().has_send())
    {
        LOG_DEBUG << "Replaying send message.";
        auto replaymsg = msg.message().send();

        Winsock::sendFnc sendTrampoline = (Winsock::sendFnc)GetCallAddress(Winsock::WinsockFunction::send);

        bytesSent = sendTrampoline((SOCKET)replaymsg.socket(), replaymsg.buffer().c_str(), replaymsg.length(),
            replaymsg.flags());
    }
    if (msg.message().has_sendto())
    {
        LOG_DEBUG << "Replaying sendto message.";
        auto replaymsg = msg.message().sendto();

        Winsock::sendtoFnc sendtoTrampoline = (Winsock::sendtoFnc)GetCallAddress(Winsock::WinsockFunction::sendto);

        bytesSent = sendtoTrampoline((SOCKET)replaymsg.socket(), replaymsg.buffer().c_str(), replaymsg.length(),
            replaymsg.flags(), (const sockaddr *)replaymsg.to_address(), replaymsg.to_length());
    }
    if (msg.message().has_wsasend())
    {
        LOG_DEBUG << "Replaying WSASend message.";
        auto replaymsg = msg.message().wsasend();

        std::unique_ptr<WSABUF[], decltype(wsabufDeleter)> wsaBufs(new WSABUF[replaymsg.count()], wsabufDeleter);
        for (int i = 0; i < replaymsg.count(); ++i)
        {
            wsaBufs[i].buf = (CHAR *)replaymsg.buffers(i).c_str();
            wsaBufs[i].len = replaymsg.buffer_size(i);
        }

        Winsock::WSASendFnc WSASendTrampoline = (Winsock::WSASendFnc)GetCallAddress(Winsock::WinsockFunction::WSASend);

        bytesSent = WSASendTrampoline((SOCKET)replaymsg.socket(), wsaBufs.get(), replaymsg.count(), (LPDWORD)replaymsg.bytes_sent_address(),
            replaymsg.flags(), (LPWSAOVERLAPPED)replaymsg.overlapped_address(),
            (LPOVERLAPPED_COMPLETION_ROUTINE)replaymsg.overlapped_routine_address());

    }
    if (msg.message().has_wsasendto())
    {
        LOG_DEBUG << "Replaying WSASendTo message.";
        auto replaymsg = msg.message().wsasendto();

        std::unique_ptr<WSABUF[], decltype(wsabufDeleter)> wsaBufs(new WSABUF[replaymsg.count()], wsabufDeleter);
        for (int i = 0; i < replaymsg.count(); ++i)
        {
            wsaBufs[i].buf = (CHAR *)replaymsg.buffers(i).c_str();
            wsaBufs[i].len = replaymsg.buffer_size(i);
        }

        Winsock::WSASendToFnc WSASendToTrampoline = (Winsock::WSASendToFnc)GetCallAddress(Winsock::WinsockFunction::WSASendTo);

        bytesSent = WSASendToTrampoline((SOCKET)replaymsg.socket(), wsaBufs.get(), replaymsg.count(), (LPDWORD)replaymsg.bytes_sent_address(),
            replaymsg.flags(), (const sockaddr *)replaymsg.to_address(), replaymsg.to_length(),
            (LPWSAOVERLAPPED)replaymsg.overlapped_address(), (LPWSAOVERLAPPED_COMPLETION_ROUTINE)replaymsg.overlapped_routine_address());
    }

    return bytesSent > 0;
}

}
}
