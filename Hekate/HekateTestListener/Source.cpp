#ifdef _M_IX86
#ifdef _DEBUG
#pragma comment(lib, "protobuf/lib/libprotobufd_x86.lib")
#else
#pragma comment(lib, "protobuf/lib/libprotobuf_x86.lib")
#endif
#elif defined _M_AMD64
#ifdef _DEBUG
#pragma comment(lib, "protobuf/lib/libprotobufd_x64.lib")
#else
#pragma comment(lib, "protobuf/lib/libprotobuf_x64.lib")
#endif
#else
#error "Unsupported platform"
#endif

#include <iostream>
#include <memory>
#include <string>

#include "Protobuf/Proto/Server/HekateServerProto.pb.h"
#include "Protobuf/Proto/Client/HekateClientProto.pb.h"
#include "Protobuf/Message/HekateClientMessageBuilder.h"

#include <Windows.h>

void PrintHexBuffer(const char * pBytes, const size_t ulLength)
{
    for (size_t i = 0; i < ulLength; ++i)
    {
        char byte = (char)pBytes[i] & 0xFF;
        if (byte >= 0x20 && byte <= 0x7E)
        {
            std::cout << byte << " ";
        }
        else
        {
            std::cout << "0x" << std::hex << byte << " ";
        }
    }
    std::cout << std::endl;
}

void OutputReceivedMessage(const Hekate::Protobuf::Proto::Server::HekateMessage &msg)
{
    if (!msg.has_hook_info())
    {
        std::cerr << "Missing Hekate -> client message.\n";
        exit(-1);
    }

    if (msg.hook_info().has_send())
    {
        auto hookmsg = msg.hook_info().send();
        std::cout << "Socket: 0x" << std::hex << hookmsg.socket() << std::endl;
        std::cout << "Buffer: ";
        PrintHexBuffer(hookmsg.buffer().c_str(), hookmsg.length());
        std::cout << "Length: 0x" << std::hex << hookmsg.length() << std::endl;
    }
    else if (msg.hook_info().has_sendto())
    {
        auto hookmsg = msg.hook_info().sendto();
        std::cout << "Socket: 0x" << std::hex << hookmsg.socket() << std::endl;
        std::cout << "Buffer: ";
        PrintHexBuffer(hookmsg.buffer().c_str(), hookmsg.length());
        std::cout << "Length: 0x" << std::hex << hookmsg.length() << std::endl;
        std::cout << "to struct address: 0x" << std::hex << hookmsg.to_address() << std::endl;
        std::cout << "to address length: 0x" << std::hex << hookmsg.to_length() << std::endl;
    }
    else if (msg.hook_info().has_wsasend())
    {
        auto hookmsg = msg.hook_info().wsasend();
        std::cout << "Socket: 0x" << std::hex << hookmsg.socket() << std::endl;
        for (int i = 0; i < hookmsg.count(); ++i)
        {
            std::cout << "Buffer #" << i << std::endl;
            PrintHexBuffer(hookmsg.buffers(i).c_str(), hookmsg.buffer_size(i));
        }
        std::cout << "Count: 0x" << std::hex << hookmsg.count() << std::endl;
        std::cout << "Bytes sent address: 0x" << std::hex << hookmsg.bytes_sent_address() << std::endl;
        std::cout << "Flags: 0x" << std::hex << hookmsg.flags() << std::endl;
        std::cout << "Overlapped address: 0x" << std::hex << hookmsg.overlapped_address() << std::endl;
        std::cout << "Overlapped routine address: 0x" << std::hex << hookmsg.overlapped_routine_address() << std::endl;
    }
    else if (msg.hook_info().has_wsasendto())
    {
        auto hookmsg = msg.hook_info().wsasendto();
        std::cout << "Socket: 0x" << std::hex << hookmsg.socket() << std::endl;
        for (int i = 0; i < hookmsg.count(); ++i)
        {
            std::cout << "Buffer #" << i << std::endl;
            PrintHexBuffer(hookmsg.buffers(i).c_str(), hookmsg.buffer_size(i));
        }
        std::cout << "Count: 0x" << std::hex << hookmsg.count() << std::endl;
        std::cout << "Bytes sent address: 0x" << std::hex << hookmsg.bytes_sent_address() << std::endl;
        std::cout << "Flags: 0x" << std::hex << hookmsg.flags() << std::endl;
        std::cout << "to struct address: 0x" << std::hex << hookmsg.to_address() << std::endl;
        std::cout << "to struct length: 0x" << std::hex << hookmsg.to_length() << std::endl;
        std::cout << "Overlapped address: 0x" << std::hex << hookmsg.overlapped_address() << std::endl;
        std::cout << "Overlapped routine address: 0x" << std::hex << hookmsg.overlapped_routine_address() << std::endl;

    }
    else if (msg.hook_info().has_recv())
    {
        auto hookmsg = msg.hook_info().recv();
        std::cout << "Socket: 0x" << std::hex << hookmsg.socket() << std::endl;
        std::cout << "Buffer: ";
        std::cout << "Buffer address: " << hookmsg.buffer() << std::endl;
        std::cout << "Length: 0x" << std::hex << hookmsg.length() << std::endl;
    }
    else if (msg.hook_info().has_recvfrom())
    {
        auto hookmsg = msg.hook_info().recvfrom();
        std::cout << "Socket: 0x" << std::hex << hookmsg.socket() << std::endl;
        std::cout << "Buffer: ";
        std::cout << "Buffer address: " << hookmsg.buffer() << std::endl;
        std::cout << "Length: 0x" << std::hex << hookmsg.length() << std::endl;
        std::cout << "from struct address: 0x" << std::hex << hookmsg.from_address() << std::endl;
        std::cout << "from length address: 0x" << std::hex << hookmsg.from_length_address() << std::endl;
    }
    else if (msg.hook_info().has_wsarecv())
    {
        auto hookmsg = msg.hook_info().wsarecv();
        std::cout << "Socket: 0x" << std::hex << hookmsg.socket() << std::endl;
        for (int i = 0; i < hookmsg.count(); ++i)
        {
            std::cout << "Buffer #" << i << std::endl;
            std::cout << "Buffer address: " << hookmsg.buffers(i) << std::endl;
        }
        std::cout << "Count: 0x" << std::hex << hookmsg.count() << std::endl;
        std::cout << "Bytes received address: 0x" << std::hex << hookmsg.bytes_received_address() << std::endl;
        std::cout << "Flags address: 0x" << std::hex << hookmsg.flags_address() << std::endl;
        std::cout << "Overlapped address: 0x" << std::hex << hookmsg.overlapped_address() << std::endl;
        std::cout << "Overlapped routine address: 0x" << std::hex << hookmsg.overlapped_routine_address() << std::endl;
    }
    else if (msg.hook_info().has_wsarecvfrom())
    {
        auto hookmsg = msg.hook_info().wsarecvfrom();
        std::cout << "Socket: 0x" << std::hex << hookmsg.socket() << std::endl;
        for (int i = 0; i < hookmsg.count(); ++i)
        {
            std::cout << "Buffer #" << i << std::endl;
            std::cout << "Buffer address: " << hookmsg.buffers(i) << std::endl;
        }
        std::cout << "Count: 0x" << std::hex << hookmsg.count() << std::endl;
        std::cout << "Bytes received address: 0x" << std::hex << hookmsg.bytes_received_address() << std::endl;
        std::cout << "Flags address: 0x" << std::hex << hookmsg.flags_address() << std::endl;
        std::cout << "from struct address: 0x" << std::hex << hookmsg.from_address() << std::endl;
        std::cout << "from struct length: 0x" << std::hex << hookmsg.from_length_address() << std::endl;
        std::cout << "Overlapped address: 0x" << std::hex << hookmsg.overlapped_address() << std::endl;
        std::cout << "Overlapped routine address: 0x" << std::hex << hookmsg.overlapped_routine_address() << std::endl;
    }
    else if(msg.has_acknowledge())
    {
        std::cout << "Received acknowledge message from server." << std::endl;
        std::cout << "Code: " << msg.acknowledge().debug_response_id() << std::endl;
    }
    else if (msg.hook_info().has_filter_message())
    {
        auto filtermsg = msg.hook_info().filter_message();
        std::cout << "Received filter hit message." << std::endl;
        std::cout << "Filter hit id: 0x" << std::hex << filtermsg.filter_id() << std::endl;
        if (filtermsg.broken())
        {
            std::cout << "Application is currently in a paused state." << std::endl;
        }
    }
    else
    {
        std::cerr << "Received unknown/empty message from server." << std::endl;
    }
}

DWORD WINAPI ReadLoop(LPVOID lpParameter)
{
    HANDLE hPipeOut = (HANDLE)lpParameter;

    std::cout << "Starting read loop." << std::endl;
    
    BOOL bRet = false;
    do
    {
        DWORD dwSize = 0;
        DWORD dwBytesRead = 0;
        bRet = ReadFile(hPipeOut, &dwSize, sizeof(DWORD), &dwBytesRead, nullptr);

        if (bRet != 0 && dwBytesRead > 0)
        {
            char *pBytes = new char[dwSize];
            bRet = ReadFile(hPipeOut, pBytes, dwSize, &dwBytesRead, nullptr);
            if (bRet != 0)
            {
                Hekate::Protobuf::Proto::Server::HekateMessage msg;
                bool bCanParse = msg.ParseFromArray(pBytes, dwSize);
                if (bCanParse)
                {
                    std::cout << "Successfully parsed Hekate message." << std::endl;
                    OutputReceivedMessage(msg);
                }
                else
                {
                    std::cerr << "Could not parse received Hekate message." << std::endl;
                }
            }
            else
            {
                std::cerr << "Failed to read message. " 
                    << "Error = 0x" << std::hex << GetLastError();
            }
        }
        else
        {
            std::cerr << "Failed to read message. " 
                << "Error = 0x" << std::hex << GetLastError();
        }
    } while (bRet != 0);

    return 0;
}

void WriteMessage(const HANDLE hPipe,
    const Hekate::Protobuf::Proto::Client::HekateClientMessage &msg)
{
    std::unique_ptr<char> pBytes(new char[msg.ByteSize()]);
    (void)msg.SerializePartialToArray(pBytes.get(), msg.ByteSize());

    DWORD dwBytesWritten = 0;
    DWORD dwSize = msg.ByteSize();
    BOOL bRet = WriteFile(hPipe, (LPCVOID)&dwSize, sizeof(DWORD), &dwBytesWritten, nullptr);
    if (bRet == 0)
    {
        std::cerr << "Could not send data length to server. "
            << "Error = 0x" << std::hex << GetLastError();
        exit(-1);
    }
    bRet = WriteFile(hPipe, pBytes.get(), dwSize, &dwBytesWritten, nullptr);
    if (bRet == 0)
    {
        std::cerr << "Could not send message to server. "
            << "Error = 0x" << std::hex << GetLastError();
        exit(-1);
    }
}

int main(int argc, char *argv[])
{
    GOOGLE_PROTOBUF_VERIFY_VERSION;

    const std::string strPipeIn("\\\\.\\pipe\\HekatePipeInbound");
    const std::string strPipeOut("\\\\.\\pipe\\HekatePipeOutbound");

    std::cout << "Starting pipe connections.\n";

    HANDLE hPipeIn = CreateFileA(strPipeIn.c_str(), GENERIC_WRITE, 0, nullptr,
        OPEN_EXISTING, 0, nullptr);
    if (hPipeIn == INVALID_HANDLE_VALUE)
    {
        std::cerr << "Could not open " << strPipeIn
            << "Error = 0x" << std::hex << GetLastError() << std::endl;
        exit(-1);
    }

    HANDLE hPipeOut = CreateFileA(strPipeOut.c_str(), GENERIC_READ, 0, nullptr,
        OPEN_EXISTING, 0, nullptr);
    if (hPipeOut == INVALID_HANDLE_VALUE)
    {
        std::cerr << "Could not open " << strPipeOut << std::endl
            << "Error = 0x" << std::hex << GetLastError() << std::endl;
        exit(-1);
    }

    std::cout << "Pipes connected. Changing read mode." << std::endl;

    DWORD dwMode = PIPE_READMODE_MESSAGE;
    BOOL bRet = SetNamedPipeHandleState(hPipeOut, &dwMode, nullptr, nullptr);
    if (bRet != 0)
    {
        std::cerr << "Could not change read mode on " << strPipeOut
            << "Error = 0x" << std::hex << GetLastError() << std::endl;      
        exit(-1);
    }

    std::cout << "Creating read loop thread." << std::endl;

    DWORD dwThreadId = 0;
    HANDLE hThread = CreateThread(nullptr, 0, ReadLoop, (LPVOID)hPipeOut, 0, &dwThreadId);
    if (hThread == INVALID_HANDLE_VALUE)
    {
        std::cerr << "Could not create read loop thread." << std::endl
            << "Error = 0x" << std::hex << GetLastError() << std::endl;
        exit(-1);
    }

    std::cout << "Created read thread." << std::endl;

    WriteMessage(hPipeIn,
        Hekate::Protobuf::Message::HekateClientMessageBuilder::CreateAddHookMessage("send"));
    WriteMessage(hPipeIn,
        Hekate::Protobuf::Message::HekateClientMessageBuilder::CreateAddHookMessage("sendto"));
    WriteMessage(hPipeIn,
        Hekate::Protobuf::Message::HekateClientMessageBuilder::CreateAddHookMessage("WSASend"));
    WriteMessage(hPipeIn,                                                          
        Hekate::Protobuf::Message::HekateClientMessageBuilder::CreateAddHookMessage("WSASendTo"));
    WriteMessage(hPipeIn,
        Hekate::Protobuf::Message::HekateClientMessageBuilder::CreateAddHookMessage("recv"));
    WriteMessage(hPipeIn,
        Hekate::Protobuf::Message::HekateClientMessageBuilder::CreateAddHookMessage("recvfrom"));
    WriteMessage(hPipeIn,
        Hekate::Protobuf::Message::HekateClientMessageBuilder::CreateAddHookMessage("WSARecv"));
    WriteMessage(hPipeIn,
        Hekate::Protobuf::Message::HekateClientMessageBuilder::CreateAddHookMessage("WSARecvFrom"));

    WaitForSingleObject(hThread, INFINITE);

    std::cout << std::endl << "Exiting." << std::endl;

    CloseHandle(hPipeIn);
    CloseHandle(hPipeOut);

    google::protobuf::ShutdownProtobufLibrary();

    return 0;
}