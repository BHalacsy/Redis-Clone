#include <parser.hpp>
#include <vector>
#include <string>
#include <iostream>
#include <util.hpp>

std::vector<std::string> parseRESP(const char datatype, const int sock)
{
    switch (datatype)
    {
        case '+': return parseSimpleString(sock);
        case '-': return parseError(sock);
        case ':': return parseInteger(sock);
        case '$': return parseBulkString(sock);
        case '*': return parseArray(sock);
        default:
            throw std::runtime_error("RESP type not yet implemented or handled");
    }
}

std::vector<std::string> parseSimpleString(const int sock)
{
    return {readLine(sock)};
}

std::vector<std::string> parseError(const int sock)
{
    return splitSpaces(readLine(sock));
}

std::vector<std::string> parseInteger(const int sock)
{
    return {readLine(sock)};
}

std::vector<std::string> parseBulkString(const int sock)
{
    std::string lenStr = readLine(sock);
    int len = std::stoi(lenStr);
    if (len == -1) return {};

    std::string retStr;
    retStr = readLine(sock);
    if (retStr.size() != len) throw std::runtime_error("Malformed bulk string");

    return {retStr};
}

std::vector<std::string> parseArray(const int sock)
{
    std::string lenStr = readLine(sock);
    int arrayLen = std::stoi(lenStr);
    std::vector<std::string> ret;
    for (ssize_t i = 0; i < arrayLen - 1; i++)
    {
        char type = readByte(sock);
        std::vector<std::string> element = parseRESP(type, sock);
        ret.insert(ret.end(), element.begin(), element.end());
    }
    return ret;
}

std::string parseCommandToRESP(const std::string& command)
{
    std::vector<std::string> parts = splitSpaces(command);
    std::string retStr = "*" + std::to_string(parts.size()) + "\r\n";
    for (const auto& i : parts)
    {
        retStr += "$" + std::to_string(i.size()) + "\r\n" + i + "\r\n";
    }
    return retStr;
}
