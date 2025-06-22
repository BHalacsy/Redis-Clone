#include <vector>
#include <string>
#include <format>

#include <parser.hpp>
#include <util.hpp>

//TODO make sure pipelining works (check for incomplete commands)
std::vector<std::string> parseRESP(const char* buffer, size_t len, size_t& offset)
{
    if (offset >= len) throw std::runtime_error("Nothing to parse");
    char datatype = buffer[offset];
    switch (datatype)
    {
        case '+': return parseSimpleString(buffer, len, offset);
        case '-': return parseError(buffer, len, offset);
        case ':': return parseInteger(buffer, len, offset);
        case '$': return parseBulkString(buffer, len, offset);
        case '*': return parseArray(buffer, len, offset);
        default: throw std::runtime_error("RESP type not yet implemented or handled");
    }
}

std::vector<std::string> parseSimpleString(const char* buffer, size_t len, size_t& offset)
{
    return {readLine(buffer, len, offset)};
}

std::vector<std::string> parseError(const char* buffer, size_t len, size_t& offset)
{
    return splitSpaces(readLine(buffer, len, offset));
}

std::vector<std::string> parseInteger(const char* buffer, size_t len, size_t& offset)
{
    return {readLine(buffer, len, offset)};
}

std::vector<std::string> parseBulkString(const char* buffer, size_t len, size_t& offset)
{
    std::string lenStr = readLine(buffer, len, offset);
    int lenNum = std::stoi(lenStr);
    if (lenNum == -1) return {};
    if (offset + lenNum + 2 > len) throw std::runtime_error("Incomplete bulk string");
    std::string retStr = readLine(buffer, len, offset);
    offset += lenNum;
    if (retStr.size() != len) throw std::runtime_error("Malformed bulk string");

    return {retStr};
}

std::vector<std::string> parseArray(const char* buffer, size_t len, size_t& offset)
{
    std::string lenStr = readLine(buffer, len, offset);
    int arrayLen = std::stoi(lenStr);
    std::vector<std::string> ret;
    for (ssize_t i = 0; i < arrayLen; i++)
    {
        if (offset >= len) throw std::runtime_error("Incomplete array element");
        std::vector<std::string> element = parseRESP(buffer, len, offset);
        if (!element.empty()) ret.push_back(element[0]);
    }
    return ret;
}

std::string parseCommandToRESP(const std::string& command)
{
    const std::vector<std::string> parts = splitSpaces(command);
    std::string retStr = std::format("*{}\r\n", parts.size());;
    for (const auto& i : parts)
    {
        retStr += std::format("${}\r\n{}\r\n", i.size(), i);
    }
    return retStr;
}

int intParser(const std::string& respInt) {
    size_t end = respInt.find("\r\n");
    return std::stoi(respInt.substr(1, end - 1));
}