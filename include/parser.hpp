#pragma once

#include <tuple>
#include <vector>
#include <string>
#include <RESPtype.hpp>


std::vector<std::string> parseRESP(char datatype, int sock);
std::vector<std::string> parseSimpleString(int sock);
std::vector<std::string> parseError(int sock);
std::vector<std::string> parseInteger(int sock);
std::vector<std::string> parseBulkString(int sock);
std::vector<std::string> parseArray(int sock);

std::string parseCommandToRESP(const std::string& command); // not sure yet