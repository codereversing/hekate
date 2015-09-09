#include "IPC/IPCBase.h"

#include "plog/Log.h"

namespace Hekate
{
namespace IPC
{

std::mutex IPCBase::m_ipcMutex;

IPCBase::IPCBase(IPCRecvCallback recvCallback) :
    m_recvCallback{ recvCallback },
    m_bIsConnected { false }
{
}

const bool IPCBase::Open()
{
    std::lock_guard<std::mutex> lock(m_ipcMutex);

    if (IsConnected())
    {
        LOG_ERROR << "An IPC instance is already active. Cannot create another one.";
        return false;
    }

    return OpenImpl();
}

const bool IPCBase::Close()
{
    std::lock_guard<std::mutex> lock(m_ipcMutex);

    if (!IsConnected())
    {
        LOG_ERROR << "No IPC instance is active. Cannot close.";
        return false;
    }

    return CloseImpl();
}

const bool IPCBase::Send(const char * const bytes, const int size)
{
    std::lock_guard<std::mutex> lock(m_ipcMutex);

    if (!IsConnected())
    {
        LOG_ERROR << "Cannot sent bytes since there is no IPC connection.";
        return false;
    }

    return SendImpl(bytes, size);
}

const bool IPCBase::IsConnected() const
{
    return m_bIsConnected.load();
}

}
}