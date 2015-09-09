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

HANDLE hPipeIn = INVALID_HANDLE_VALUE;

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
    else if (msg.hook_info().has_recv())
    {
        auto hookmsg = msg.hook_info().recv();
        std::cout << "Socket: 0x" << std::hex << hookmsg.socket() << std::endl;
        std::cout << "Buffer: ";
        std::cout << "Buffer address: " << hookmsg.buffer() << std::endl;
        std::cout << "Length: 0x" << std::hex << hookmsg.length() << std::endl;
    }
    else if (msg.has_acknowledge())
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
            std::cout << "Application is currently in a paused state. "
                << "Sleeping for five seconds and resuming it." << std::endl;
            Sleep(5000);
            auto contMsg = Hekate::Protobuf::Message::HekateClientMessageBuilder::CreateContinueMessage();
            WriteMessage(hPipeIn, contMsg);
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

int main(int argc, char *argv[])
{
    GOOGLE_PROTOBUF_VERIFY_VERSION;

    const std::string strPipeIn("\\\\.\\pipe\\HekatePipeInbound");
    const std::string strPipeOut("\\\\.\\pipe\\HekatePipeOutbound");

    std::cout << "Starting pipe connections.\n";

    hPipeIn = CreateFileA(strPipeIn.c_str(), GENERIC_WRITE, 0, nullptr,
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

    auto firstFilter = Hekate::Protobuf::Message::HekateClientMessageBuilder::CreateSubstringFilterMessage(0x123,
        false, "first", 5);
    int replacementIndices1[] = { 12, 13, 14, 15, 16 };
    Hekate::Protobuf::Message::HekateClientMessageBuilder::AddSubstituteMessage(firstFilter, "QWERT", replacementIndices1, 5);
    WriteMessage(hPipeIn, firstFilter);

    int replacementIndices2[] = { 12, 13, 14, 15, 16, 17 };
    auto secondFilter = Hekate::Protobuf::Message::HekateClientMessageBuilder::CreateByteFilterMessage(0x456,
        false, "second", replacementIndices2, 6);
    const char *replace = "This is an entirely new message.";
    Hekate::Protobuf::Message::HekateClientMessageBuilder::AddReplacementMessage(secondFilter, replace, (int)strlen(replace));
    WriteMessage(hPipeIn, secondFilter);

    int replacementIndices3[] = { 0 };
    auto secondFilterSize = Hekate::Protobuf::Message::HekateClientMessageBuilder::CreateByteFilterMessage(0x457,
        false, "\x19", replacementIndices3, 1);
    Hekate::Protobuf::Message::HekateClientMessageBuilder::AddReplacementMessage(secondFilterSize, "\x20", 1);
    WriteMessage(hPipeIn, secondFilterSize);

    auto thirdFilter = Hekate::Protobuf::Message::HekateClientMessageBuilder::CreateLengthFilterMessage(0x789,
        true, 30, Hekate::Protobuf::Proto::Client::LengthType::GREATER);
    WriteMessage(hPipeIn, thirdFilter);

    WriteMessage(hPipeIn,
        Hekate::Protobuf::Message::HekateClientMessageBuilder::CreateAddHookMessage("send"));

    WaitForSingleObject(hThread, INFINITE);

    std::cout << std::endl << "Exiting." << std::endl;

    CloseHandle(hPipeIn);
    CloseHandle(hPipeOut);

    google::protobuf::ShutdownProtobufLibrary();

    return 0;
}