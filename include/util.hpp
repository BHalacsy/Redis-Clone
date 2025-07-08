#pragma once
#include <sys/socket.h>
#include <string>
#include <stdexcept>
#include <sstream>
#include <iostream>
#include <vector>
#include <format>
#include <fstream>
#include <unordered_map>
#include <sys/sysinfo.h>
#include <unistd.h>
#include <tbb/concurrent_hash_map.h>

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


inline std::string readLine(const char* buffer, const size_t len, size_t& offset)
{
    const size_t readStart = offset;
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


//For serialization (to work with Boost)
inline std::unordered_map<std::string, RESPValue> convertToUnorderedMap(const tbb::concurrent_hash_map<std::string, RESPValue>& conMap)
{
    std::unordered_map<std::string, RESPValue> retMap;
    for (const auto & [key, val] : conMap)
    {
        retMap[key] = val;
    }
    return retMap;
}

inline tbb::concurrent_hash_map<std::string, RESPValue> convertToConcurrentMap(const std::unordered_map<std::string, RESPValue>& conMap)
{
    tbb::concurrent_hash_map<std::string, RESPValue> retMap;
    for (const auto & [key, val] : conMap)
    {
        retMap.insert({key, val});
    }
    return retMap;
}

//Memory management

// inline unsigned int getMemoryLimit()
// {
//     struct sysinfo info;
//     if (sysinfo(&info) == 0)
//     {
//         // Return total RAM in bytes
//         return static_cast<unsigned int>(info.totalram * info.mem_unit);
//     }
//     return 0;
// }

// inline unsigned int getMemoryUsage()
// {
//     std::ifstream statm("/proc/self/statm");
//     long pages = 0;
//     if (statm >> pages)
//     {
//         long page_size = sysconf(_SC_PAGESIZE);
//         return static_cast<unsigned int>(pages * page_size); //Bytes
//     }
//     return 0;
// }