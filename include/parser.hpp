#pragma once

#include <vector>
#include <string>
#include "RESPtype.hpp"


std::vector<std::string> parseRESP(const char* buffer, size_t len, size_t& offset);
std::vector<std::string> parseSimpleString(const char* buffer, size_t len, size_t& offset);
std::vector<std::string> parseError(const char* buffer, size_t len, size_t& offset);
std::vector<std::string> parseInteger(const char* buffer, size_t len, size_t& offset);
std::vector<std::string> parseBulkString(const char* buffer, size_t len, size_t& offset);
std::vector<std::string> parseArray(const char* buffer, size_t len, size_t& offset);

std::string parseCommandToRESP(const std::string& command); // not sure yet
int intParser(const std::string& line);