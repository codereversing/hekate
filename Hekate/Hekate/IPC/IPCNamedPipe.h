#pragma once

#include "Common/SafeObject.h"
#include "IPC/IPCBase.h"

#include <string>
#include <thread>

namespace Hekate
{
namespace IPC
{

class IPCNamedPipe final : public IPCBase
{
public:
    IPCNamedPipe(const std::string pipeNameOutbound, const std::string pipeNameInbound,
        IPCRecvCallback recvCallback);
    ~IPCNamedPipe();

private:
    const bool OpenImpl();
    const bool CloseImpl();
    const bool SendImpl(const char * const bytes, const int size);

    const bool InitializePipes();
    const bool ConnectPipes();

    void ListenEntry();
    void RecvLoop();

    std::string m_pipeNameOutbound;
    std::string m_pipeNameInbound;

    std::thread m_listenThread;
    std::thread m_recvThread;

    std::atomic_bool m_bContinueListening;

    Hekate::Common::SafeHandle m_pipeOutbound;
    Hekate::Common::SafeHandle m_pipeInbound;
};

}
}