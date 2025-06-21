#pragma once
#include <sys/socket.h>
#include <string>
#include <stdexcept>
#include <sstream>
#include <iostream>


inline char readByte(const int sock)
{
    char byte;
    if (recv(sock, &byte, 1, 0) <= 0)
    {
        throw std::runtime_error("Connection closed (readByte)");
    }
    return byte;
}

inline std::string readLine(const int sock)
{
    std::string retString;
    while (true)
    {
        const char byte = readByte(sock);
        if (byte == '\r')
        {
            if (readByte(sock) != '\n')
            {
                throw std::runtime_error("Malformed readLine");
            }
            return retString;
        }
        retString.push_back(byte);
    }
}

inline std::vector<std::string> splitSpaces(const std::string& line)
{
    std::vector<std::string> retVec;
    std::stringstream ss(line);
    std::string wordToken;
    while (ss >> wordToken)
    {
        retVec.push_back(wordToken);
    }
    return retVec;
}

inline std::string argumentError(std::string expected, int got)
{
    return std::format("-ERR command expected {} arguments, got {} instead\r\n", expected, got);
}