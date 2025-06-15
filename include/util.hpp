#pragma once
#include <sys/socket.h>
#include <string>
#include <stdexcept>

inline char readByte(int sock)
{
    char byte;
    if (recv(sock, &byte, 1, 0) <= 0)
    {
        throw std::runtime_error("Connection closed (readByte)");
    }
    return byte;
}

inline std::string readLine(int sock)
{
    std::string retString;
    while (true)
    {
        char byte = readByte(sock);
        if (byte == '\r')
        {
            if (readByte(sock) != '\n')
            {
                throw std::runtime_error("Connection closed (readLine)");
            }
            return retString;
        }
        retString.push_back(byte);
    }
}
