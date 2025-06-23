#pragma once

#include <string>
#include <kvstore.hpp>

//commands for switching:
//TODO implement INCR, DCR, MGET, PING, ECHO, EXPIRE TTL, FLUSHALL...
enum class Commands
{
    PING, ECHO,
    SET, GET, DEL, EXISTS,
    INCR, DCR, EXPIRE, TTL, FLUSHALL, MGET,
    LPUSH, RPUSH, LPOP, RPOP, LRANGE, LLEN, LINDEX, LSET, LREM,
    //LMOVE??

    UNKNOWN
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
std::string handleExpire(KVStore& kvstore, const std::vector<std::string>& args);
std::string handleTTL(KVStore& kvstore, const std::vector<std::string>& args);
std::string handleFlushall(KVStore& kvstore, const std::vector<std::string>& args);
std::string handleMget(KVStore& kvstore, const std::vector<std::string>& args);
//TODO LIST commands
std::string handleLpush(KVStore& kvstore, const std::vector<std::string>& args);
std::string handleRpush(KVStore& kvstore, const std::vector<std::string>& args);
std::string handleLpop(KVStore& kvstore, const std::vector<std::string>& args);
std::string handleRpop(KVStore& kvstore, const std::vector<std::string>& args);
std::string handleLrange(KVStore& kvstore, const std::vector<std::string>& args);
std::string handleLlen(KVStore& kvstore, const std::vector<std::string>& args);
std::string handleLindex(KVStore& kvstore, const std::vector<std::string>& args);
std::string handleLset(KVStore& kvstore, const std::vector<std::string>& args);
std::string handleLrem(KVStore& kvstore, const std::vector<std::string>& args);


