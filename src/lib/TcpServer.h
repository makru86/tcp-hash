#pragma once
#include <string>

namespace app
{

class TcpServer
{
    std::string _text;

public:
    explicit TcpServer(const std::string& text);

    std::string appendIt(const std::string& extra) const;
    std::string text() const;
};

}