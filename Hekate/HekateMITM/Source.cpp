#pragma comment(lib, "Ws2_32.lib")

#define WIN32_LEAN_AND_MEAN

#include <stdio.h>
#include <thread>
#include <Windows.h>
#include <WinSock2.h>
#include <WS2tcpip.h>

int main(int argc, char *argv[])
{
    WSADATA wsaData = { 0 };
    int iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (iResult != 0)
    {
        fprintf(stderr, "WSAStartup failed. Error = 0x%X\n",
            iResult);
        return -1;
    }

    struct addrinfo hints = { 0 };
    struct addrinfo *result = nullptr;
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;
    hints.ai_flags = AI_PASSIVE;

    iResult = getaddrinfo(nullptr, "6789", &hints, &result);
    if (iResult != 0)
    {
        fprintf(stderr, "getaddrinfo failed. Error = 0x%X\n",
            iResult);
        return -1;
    }

    SOCKET listenSocket = socket(result->ai_family, result->ai_socktype,
        result->ai_protocol);
    if (listenSocket == INVALID_SOCKET)
    {
        fprintf(stderr, "socket failed. Error = 0x%X\n", WSAGetLastError());
        return -1;
    }

    iResult = bind(listenSocket, result->ai_addr, (int)result->ai_addrlen);
    if (iResult == SOCKET_ERROR)
    {
        fprintf(stderr, "bind failed. Error = 0x%X\n", WSAGetLastError());
        return -1;
    }

    freeaddrinfo(result);

    iResult = listen(listenSocket, SOMAXCONN);
    if (iResult == SOCKET_ERROR)
    {
        fprintf(stderr, "listen failed. Error = 0x%X\n", WSAGetLastError());
        return -1;
    }

    std::thread([&]()
    {
        SOCKET clientSocket = accept(listenSocket, nullptr, nullptr);
        if (clientSocket == INVALID_SOCKET)
        {
            fprintf(stderr, "accept failed. Error = 0x%X\n", WSAGetLastError());
            exit(-1);
        }

        closesocket(listenSocket);

        int recvSize = 0;
        char recvBuffer[64] = { 0 };
        do
        {
            iResult = recv(clientSocket, (char *)&recvSize, sizeof(int), 0);
            if (iResult == 0 || recvSize == 0)
            {
                fprintf(stderr, "Did not receive size. Error = 0x%X\n",
                    WSAGetLastError());
                exit(-1);
            }
            iResult = recv(clientSocket, recvBuffer, recvSize, 0);
            if (iResult == 0)
            {
                fprintf(stderr, "Did not receive message. Error = 0x%X\n",
                    WSAGetLastError());
                exit(-1);
            }

            fprintf(stderr, "Received: %.*s\n", recvSize, recvBuffer);

            memset(recvBuffer, 0, sizeof(recvBuffer));
        } while (iResult > 0);

        fprintf(stderr, "Exiting.\n");
        exit(0);

    }).detach();

    std::thread([&]()
    {
        int iResult = getaddrinfo("localhost", "6789", &hints, &result);
        if (iResult != 0)
        {
            fprintf(stderr, "client getaddrinfo failed. Error = 0x%x\n",
                iResult);
            exit(-1);
        }

        SOCKET connectSocket = INVALID_SOCKET;
        for (struct addrinfo *p = result; p != nullptr; p = p->ai_next)
        {
            connectSocket = socket(p->ai_family, p->ai_socktype, p->ai_protocol);
            if (connectSocket == INVALID_SOCKET)
            {
                fprintf(stderr, "socket failed. Error = 0x%X\n",
                    WSAGetLastError());
                exit(-1);
            }

            iResult = connect(connectSocket, p->ai_addr, (int)p->ai_addrlen);
            if (iResult == SOCKET_ERROR)
            {
                closesocket(connectSocket);
                connectSocket = INVALID_SOCKET;
                continue;
            }

            break;
        }

        freeaddrinfo(result);

        if (connectSocket == INVALID_SOCKET)
        {
            fprintf(stderr, "Unable to connect to server.\n");
            exit(-1);
        }

        char *buffers[] =
        {
            "This is the first buffer",
            "This is the second buffer",
            "This is the third and last buffer"
        };

        int i = 0;
        for (;;)
        {
            int length = (int)strlen(buffers[i % 3]);

            iResult = send(connectSocket, (const char *)&length, sizeof(int), 0);
            if (iResult == SOCKET_ERROR)
            {
                fprintf(stderr, "send size failed. Error = 0x%X\n",
                    WSAGetLastError());
                exit(-1);
            }

            iResult = send(connectSocket, buffers[i], length, 0);
            if (iResult == SOCKET_ERROR)
            {
                fprintf(stderr, "send message failed. Error = 0x%X\n",
                    WSAGetLastError());
                exit(-1);
            }

            i = (i + 1) % 3;
            Sleep(2000);
        }

    }).join();

    return 0;
}