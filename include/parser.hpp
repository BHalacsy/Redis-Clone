#pragma once

#include <tuple>
#include <vector>
#include <string>
#include <RESPtype.hpp>


RESPValue parseRESP(char datatype, int sock);
RESPValue parseSimpleString(int sock);
RESPValue parseError(int sock);
RESPValue parseInteger(int sock);
RESPValue parseBulkString(int sock);
RESPValue parseArray(int sock);

std::string parseCliToRESP(); // not sure yet