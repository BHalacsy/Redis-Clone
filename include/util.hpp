#pragma once
#include <sys/socket.h>
#include <string>
#include <stdexcept>
#include <sstream>
#include <iostream>
#include <vector>

#include "kvstore.hpp"


// inline char readByteSock(const int sock)
// {
//     char byte;
//     if (recv(sock, &byte, 1, 0) <= 0)
//     {
//         throw std::runtime_error("Connection closed (readByte)");
//     }
//     return byte;
// }

// inline std::string readLineSock(const int sock)
// {
//     std::string retString;
//     while (true)
//     {
//         const char byte = readByteSock(sock);
//         if (byte == '\r')
//         {
//             if (readByteSock(sock) != '\n')
//             {
//                 throw std::runtime_error("Malformed readLine");
//             }
//             return retString;
//         }
//         retString.push_back(byte);
//     }
// }


inline std::string readLine(const char* buffer, size_t len, size_t& offset)
{
    size_t readStart = offset;
    while (offset + 1 < len)
    {
        if (buffer[offset] == '\r' && buffer[offset + 1] == '\n') {
            std::string line(buffer + readStart, offset - readStart);
            offset += 2;
            return line;
        }
        offset++;
    }
    throw std::runtime_error("Incomplete command in readLine");
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

inline std::string argumentError(std::string expected, size_t got)
{
    return std::format("-ERR command expected {} arguments, got {} instead\r\n", expected, got);
}

inline std::optional<std::string> checkTypeError(KVStore& kvstore, const std::string& key, storeType expected) {
    auto type = kvstore.getType(key);
    if (type && *type != expected) return "-ERR wrong type\r\n";
    return std::nullopt;
}