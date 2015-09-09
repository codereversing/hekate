#pragma once

#include <atomic>
#include <functional>
#include <memory>
#include <mutex>

namespace Hekate
{
namespace IPC
{

using IPCRecvCallback = std::function<void(const std::unique_ptr<char[]> &, const int)>;

class IPCBase
{
public:
    IPCBase() = delete;

    IPCBase(IPCRecvCallback recvCallback);
    virtual ~IPCBase() = default;

    const bool Open();
    const bool Close();
    const bool Send(const char * const bytes, const int size);

    const bool IsConnected() const;

private:
    virtual const bool OpenImpl() = 0;
    virtual const bool CloseImpl() = 0;
    virtual const bool SendImpl(const char * const bytes, const int size) = 0;

    static std::mutex m_ipcMutex;

protected:
    std::atomic_bool m_bIsConnected;
    IPCRecvCallback m_recvCallback;
};

}
}