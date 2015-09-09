#include "IPC/IPCNamedPipe.h"

#include "plog/Log.h"

namespace Hekate
{
namespace IPC
{

IPCNamedPipe::IPCNamedPipe(const std::string pipeNameOutbound, const std::string pipeNameInbound, 
    IPCRecvCallback recvCallback) :
    IPCBase(recvCallback),
    m_pipeNameOutbound{ pipeNameOutbound },
    m_pipeNameInbound{ pipeNameInbound },
    m_bContinueListening { true }
{
    LOG_DEBUG << "Initializing IPC with named pipes.\n"
        << "Inbound name: " << pipeNameInbound.c_str() << '\n'
        << "Outbound name: " << pipeNameOutbound.c_str();

}

IPCNamedPipe::~IPCNamedPipe()
{
    LOG_DEBUG << "Destroying IPC named pipe instance.";

    if (IsConnected())
    {
        (void)CloseImpl();
    }
}

const bool IPCNamedPipe::OpenImpl()
{
    LOG_DEBUG << "Creating thread to initialize and connect named pipes.";
    
    m_listenThread = std::thread(&IPCNamedPipe::ListenEntry, this);
    m_listenThread.detach();

    return true;
}

const bool IPCNamedPipe::CloseImpl()
{
    LOG_DEBUG << "Closing named pipes.";

    m_bContinueListening.store(false);
    m_bIsConnected.store(false);

    return true;
}

const bool IPCNamedPipe::SendImpl(const char * const bytes, const int size)
{
    DWORD bytesWritten = 0;
    bool bSuccess = BOOLIFY(WriteFile(m_pipeOutbound(), &size, sizeof(int),
        &bytesWritten, nullptr));
    if (!bSuccess)
    {
        LOG_ERROR << "Send of message size to client did not successfully complete. "
            << "Error = 0x" << std::hex << GetLastError();
        return false;
    }
    if (bytesWritten != sizeof(DWORD))
    {
        LOG_ERROR << "Did not send all bytes to client.";
        return false;
    }

    bSuccess = BOOLIFY(WriteFile(m_pipeOutbound(), bytes, size, &bytesWritten, nullptr));
    if (!bSuccess)
    {
        LOG_ERROR << "Send to client did not successfully complete. "
            << "Error = 0x" << std::hex << GetLastError();
        return false;
    }
    if ((int)bytesWritten != size)
    {
        LOG_ERROR << "Did not send all bytes to client.";
        return false;
    }

    return true;
}

const bool IPCNamedPipe::InitializePipes()
{
    const DWORD dwBufferReserveSize = 0x4000;

    m_pipeOutbound = CreateNamedPipeA(m_pipeNameOutbound.c_str(), PIPE_ACCESS_OUTBOUND,
        PIPE_TYPE_MESSAGE | PIPE_READMODE_MESSAGE | PIPE_WAIT,
        PIPE_UNLIMITED_INSTANCES, dwBufferReserveSize, dwBufferReserveSize,
        0, nullptr);
    if (!m_pipeOutbound.IsValid())
    {
        LOG_ERROR << "Could not create outbound named pipe. "
            << "Error = 0x" << std::hex << GetLastError();
        return false;
    }

    m_pipeInbound = CreateNamedPipeA(m_pipeNameInbound.c_str(), PIPE_ACCESS_INBOUND,
        PIPE_TYPE_MESSAGE | PIPE_READMODE_MESSAGE | PIPE_WAIT,
        PIPE_UNLIMITED_INSTANCES, dwBufferReserveSize, dwBufferReserveSize,
        0, nullptr);
    if (!m_pipeInbound.IsValid())
    {
        LOG_ERROR << "Could not create inbound named pipe. "
            << "Error = 0x" << std::hex << GetLastError();
        return false;
    }

    LOG_DEBUG << "Inbound/outbound pipes successfully created.";

    return true;
}

const bool IPCNamedPipe::ConnectPipes()
{
    bool bConnected = BOOLIFY(ConnectNamedPipe(m_pipeOutbound(), nullptr));
    bConnected |= (GetLastError() == ERROR_PIPE_CONNECTED);
    if (!bConnected)
    {
        LOG_ERROR << "Client could not connect to outbound pipe. "
            << "Error = 0x" << std::hex << GetLastError();
        return false;
    }

    bConnected = BOOLIFY(ConnectNamedPipe(m_pipeInbound(), nullptr));
    bConnected |= (GetLastError() == ERROR_PIPE_CONNECTED);
    if (!bConnected)
    {
        LOG_ERROR << "Client could not connect to inbound pipe. "
            << "Error = 0x" << std::hex << GetLastError();
        return false;
    }

    m_bIsConnected.store(true);

    LOG_DEBUG << "Connection established to inbound/outbound pipes.";

    return true;
}

void IPCNamedPipe::ListenEntry()
{
    LOG_DEBUG << "Listen thread created.";

    while (m_bContinueListening.load())
    {

        if (InitializePipes())
        {
            LOG_DEBUG << "Waiting for connection to IPC pipe.";

            if (ConnectPipes())
            {
                LOG_DEBUG << "Successfully established connection to client. ";

                std::thread(&IPCNamedPipe::RecvLoop, this).join();

                m_bIsConnected.store(false);
            }
            else
            {
                LOG_ERROR << "Could not establish connection to pipes. "
                    << "Re-initializing pipes and trying again.";
            }
        }
        else
        {
            LOG_ERROR << "Could not initialize pipes. Retrying.";
        }
    }

    LOG_DEBUG << "Listen thread exiting";
}

void IPCNamedPipe::RecvLoop()
{
    while (m_bContinueListening.load())
    {
        LOG_DEBUG << "Entering read loop.";

        do
        {
            DWORD messageSize = 0;
            DWORD bytesRead = 0;
            bool bSuccess = BOOLIFY(ReadFile(m_pipeInbound(), &messageSize, sizeof(DWORD),
                &bytesRead, nullptr));
            if (bSuccess)
            {
                LOG_DEBUG << "Message successfully read from pipe";
                if (bytesRead != sizeof(DWORD))
                {
                    LOG_ERROR << "Read size of next message was not the expected size.";
                    return;
                }
                if (messageSize == 0)
                {
                    LOG_ERROR << "Incoming message size was read to be zero.";
                    return;
                }

                LOG_DEBUG << "Message size is 0x " << std::hex << messageSize << " bytes.";

                std::unique_ptr<char[]> pMessage(new char[messageSize]);
                bSuccess = BOOLIFY(ReadFile(m_pipeInbound(), pMessage.get(), messageSize,
                    &bytesRead, nullptr));
                if (bSuccess)
                {
                    if (bytesRead != messageSize)
                    {
                        LOG_ERROR << "Did not read message correctly."
                            << " Expected length: 0x " << std::hex << messageSize
                            << " Size read: 0x" << std::hex << bytesRead;
                        return;
                    }

                    LOG_DEBUG << "Successfully read incoming message.";
                    LOG_DEBUG << "Invoking receive callback.";

                    if (m_recvCallback != nullptr)
                    {
                        m_recvCallback(std::move(pMessage), messageSize);
                    }
                }
                else
                {
                    LOG_ERROR << "Failed to read message. "
                        << "Error = 0x" << std::hex << GetLastError();
                    return;
                }
            }
            else
            {
                LOG_ERROR << "Failed to read message. "
                    << "Error = 0x" << std::hex << GetLastError();
                return;
            }
        } while (m_bContinueListening.load());

        LOG_DEBUG << "Read loop finished.";
    }

    LOG_DEBUG << "Leaving receive message loop";
}

}
}