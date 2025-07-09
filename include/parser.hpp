#pragma once

#include <vector>
#include <string>

#include "respvalue.hpp"

std::vector<std::vector<std::string>> parseRESPPipeline(const char* buffer, size_t len, size_t& offset); //Enables pipelining
std::vector<std::string> parseRESP(const char* buffer, size_t len, size_t& offset); //Switch cases depending on RESP symbol
std::vector<std::string> parseSimpleString(const char* buffer, size_t len, size_t& offset); //+
std::vector<std::string> parseError(const char* buffer, size_t len, size_t& offset); //-ERR
std::vector<std::string> parseInteger(const char* buffer, size_t len, size_t& offset); //:
std::vector<std::string> parseBulkString(const char* buffer, size_t len, size_t& offset); //$
std::vector<std::string> parseArray(const char* buffer, size_t len, size_t& offset); //*

std::string parseCommandToRESP(const std::string& command); //Only used in client.cpp for CLI to RESP
int intParser(const std::string& line); //RESP int to int