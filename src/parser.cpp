#include <parser.hpp>
#include <tuple>
#include <vector>
#include <string>
#include <iostream>
#include <util.hpp>

RESPValue parseRESP(char datatype, int sock)
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

RESPValue parseSimpleString(int sock)
{
    return RESPValue(readLine(sock));
}

RESPValue parseError(int sock)
{

}
