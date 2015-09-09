#include "Winsock/WinsockFunction.h"

namespace Hekate
{
namespace Winsock
{

sendFnc sendAddr = nullptr;
sendtoFnc sendtoAddr = nullptr;
WSASendFnc WSASendAddr = nullptr;
WSASendToFnc WSASendToAddr = nullptr;

recvFnc recvAddr = nullptr;
recvfromFnc recvfromAddr = nullptr;
WSARecvFnc WSARecvAddr = nullptr;
WSARecvFromFnc WSARecvFromAddr = nullptr;

std::string WinsockFunctionToString(const WinsockFunction function)
{
    if (function == WinsockFunction::send)
        return "send";
    if (function == WinsockFunction::sendto)
        return "sendto";
    if (function == WinsockFunction::WSASend)
        return "WSASend";
    if (function == WinsockFunction::WSASendTo)
        return "WSASendTo";
    if (function == WinsockFunction::recv)
        return "recv";
    if (function == WinsockFunction::recvfrom)
        return "recvfrom";
    if (function == WinsockFunction::WSARecv)
        return "WSARecv";
    if (function == WinsockFunction::WSARecvFrom)
        return "WSARecvFrom";

    return "Unknown";
}

const WinsockFunction WinsockFunctionFromString(const std::string &name)
{
    if (name.compare("send") == 0)
        return WinsockFunction::send;
    if (name.compare("sendto") == 0)
        return WinsockFunction::sendto;
    if (name.compare("WSASend") == 0)
        return WinsockFunction::WSASend;
    if (name.compare("WSASendTo") == 0)
        return WinsockFunction::WSASendTo;
    if (name.compare("recv") == 0)
        return WinsockFunction::recv;
    if (name.compare("recvfrom") == 0)
        return WinsockFunction::recvfrom;
    if (name.compare("WSARecv") == 0)
        return WinsockFunction::WSARecv;
    if (name.compare("WSARecvFrom") == 0)
        return WinsockFunction::WSARecvFrom;

    return WinsockFunction::Unknown;
}

}
}