#include <vector>
#include <string>
#include <format>

#include "parser.hpp"
#include "util.hpp"

//TODO make sure pipelining works (check for incomplete commands)
std::vector<std::vector<std::string>> parseRESPPipeline(const char* buffer, const size_t len, size_t& offset)
{
    std::vector<std::vector<std::string>> commands;
    while (offset < len)
    {
        try
        {
            commands.push_back(parseRESP(buffer, len, offset));
        }
        catch (const std::runtime_error& e)
        {
            break;
        }
    }
    return commands;
}


std::vector<std::string> parseRESP(const char* buffer, size_t len, size_t& offset)
{
    if (offset >= len) throw std::runtime_error("Nothing to parse");
    const char datatype = buffer[offset];
    offset++;
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

std::vector<std::string> parseSimpleString(const char* buffer, const size_t len, size_t& offset)
{
    return {readLine(buffer, len, offset)};
}

std::vector<std::string> parseError(const char* buffer, const size_t len, size_t& offset)
{
    return splitSpaces(readLine(buffer, len, offset));
}

std::vector<std::string> parseInteger(const char* buffer, const size_t len, size_t& offset)
{
    return {readLine(buffer, len, offset)};
}

std::vector<std::string> parseBulkString(const char* buffer, const size_t len, size_t& offset)
{
    const std::string lenStr = readLine(buffer, len, offset);
    const int lenNum = std::stoi(lenStr);
    if (lenNum == -1) return {};
    if (offset + lenNum + 2 > len) throw std::runtime_error("Incomplete bulk string");

    std::string retStr = readLine(buffer, len, offset);
    if (retStr.size() != lenNum) throw std::runtime_error("Malformed bulk string");

    return {retStr};
}

std::vector<std::string> parseArray(const char* buffer, const size_t len, size_t& offset)
{
    const std::string lenStr = readLine(buffer, len, offset);
    const int arrayLen = std::stoi(lenStr);
    std::vector<std::string> ret;
    for (ssize_t i = 0; i < arrayLen; i++)
    {
        if (offset >= len || buffer[offset] == '*') throw std::runtime_error("Incomplete array element");
        std::vector<std::string> element = parseRESP(buffer, len, offset);
        if (!element.empty()) ret.push_back(element[0]);
    }
    return ret;
}

std::string parseCommandToRESP(const std::string& command) //Only for the client might move
{
    const std::vector<std::string> parts = splitSpaces(command);
    std::string retStr = std::format("*{}\r\n", parts.size());;
    for (const auto& i : parts)
    {
        retStr += std::format("${}\r\n{}\r\n", i.size(), i);
    }
    return retStr;
}

int intParser(const std::string& line) {
    const size_t end = line.find("\r\n");
    return std::stoi(line.substr(1, end - 1));
}