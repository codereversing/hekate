#pragma comment(lib, "Ws2_32.lib")

#include <WinSock2.h>
#include <Windows.h>

int main(int argc, char *argv[])
{
    while (true)
    {
        send((SOCKET)0x123, "Hello, World!", 13, 0);

        sendto((SOCKET)0x234, "sendto test message", 19, 0, nullptr, 0);

        DWORD bytesSent = 0;
        WSABUF buffers[3] =
        {
            {5, "Hello"},
            {20, "This is a WSA buffer"},
            {38, "All three of these should be displayed"}
        };
        WSASend((SOCKET)0x345, buffers, 3, &bytesSent, 0, nullptr, nullptr);

        WSASendTo((SOCKET)0x456, buffers, 3, &bytesSent, 0, nullptr, 8, nullptr, nullptr);

        char recvBuf[255] = { 0 };
        recv((SOCKET)0x567, recvBuf, 255, 0);

        recvfrom((SOCKET)0x678, recvBuf, 255, 0, nullptr, nullptr);

        DWORD bytesRecvd = 0;
        DWORD flags = 0;
        WSABUF recvBuffers[6] = { 0 };
        WSARecv((SOCKET)0x789, recvBuffers, 6, &bytesRecvd, &flags, nullptr, nullptr);

        WSARecvFrom((SOCKET)0x890, recvBuffers, 6, &bytesRecvd, &flags, nullptr, nullptr, nullptr, nullptr);

        Sleep(15000);
    }

    return 0;
}