#include "TcpServer.h"

namespace app
{

TcpServer::TcpServer(const std::string& text)
    : _text{text}
{
}

std::string TcpServer::appendIt(const std::string& extra) const
{
    return _text + " " + extra;
}

std::string TcpServer::text() const
{
    return _text;
}

}