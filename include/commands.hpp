#pragma once

#include <string>
#include <kvstore.hpp>

//commands for switching:
//TODO implement INCR, DCR, MGET, PING, ECHO, EXPIRE TTL, FLUSHALL...
enum class Commands
{
    PING, ECHO, SET, GET, DEL, EXISTS, INCR, DCR, EXPIRE, TTL, FLUSHALL, MGET, UNKNOWN
};

Commands strToCmd(const std::string& cmd);

std::string handlePing(const std::vector<std::string>& args);
std::string handleEcho(const std::vector<std::string>& args);
std::string handleSet(KVStore& kvstore, const std::vector<std::string>& args);
std::string handleGet(KVStore& kvstore, const std::vector<std::string>& args);
std::string handleDel(KVStore& kvstore, const std::vector<std::string>& args);
std::string handleExists(KVStore& kvstore, const std::vector<std::string>& args);
std::string handleIncr(KVStore& kvstore, const std::vector<std::string>& args);
std::string handleDcr(KVStore& kvstore, const std::vector<std::string>& args);
std::string handleExpire(KVStore& kvstore, const std::vector<std::string>& args);//TODO
std::string handleTTL(KVStore& kvstore, const std::vector<std::string>& args);//TODO
std::string handleFlushall(KVStore& kvstore, const std::vector<std::string>& args);//TODO
std::string handleMget(KVStore& kvstore, const std::vector<std::string>& args);//TODO


