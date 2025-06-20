#pragma once

#include <string>
#include <kvstore.hpp>

//commands for switching:
//TODO implement INCR, DCR, MGET, PING, ECHO, EXPIRE TTL, FLUSHALL...
enum class Commands
{
    SET,
    GET,
    DEL,
    EXISTS,
    UNKNOWN
};

Commands strToCmd(const std::string& cmd);

std::string handleSet(KVStore& kvstore, const std::vector<std::string>& args);
std::string handleGet(KVStore& kvstore, const std::vector<std::string>& args);
std::string handleDel(KVStore& kvstore, const std::vector<std::string>& args);
std::string handleExists(KVStore& kvstore, const std::vector<std::string>& args);

