#pragma once

#include <string>
#include "kvstore.hpp"

//commands for switching:

enum class Commands
{
    CONFIG,
    PING, ECHO,
    SET, GET, DEL, EXISTS,
    INCR, DCR, EXPIRE, TTL, FLUSHALL, MGET,
    LPUSH, RPUSH, LPOP, RPOP, LRANGE, LLEN, LINDEX, LSET, LREM, //LMOVE?
    SADD, SREM, SISMEMBER, SMEMBERS, SCARD, SPOP,
    HSET, HGET, HDEL, HEXISTS, HLEN, HKEYS, HVALS, HMGET, //HGETALL??
    MULTI, EXEC, DISCARD, WATCH, //TODO implement later
    PUBLISH, SUBSCRIBE, UNSUBSCRIBE,
    UNKNOWN
};

auto strToCmd(const std::string& cmd) -> Commands;

// Basic commands
std::string handleCONFIG(const std::vector<std::string>& args);
std::string handlePING(const std::vector<std::string>& args);
std::string handleECHO(const std::vector<std::string>& args);
std::string handleSET(KVStore& kvstore, const std::vector<std::string>& args);
std::string handleGET(KVStore& kvstore, const std::vector<std::string>& args);
std::string handleDEL(KVStore& kvstore, const std::vector<std::string>& args);
std::string handleEXISTS(KVStore& kvstore, const std::vector<std::string>& args);
std::string handleINCR(KVStore& kvstore, const std::vector<std::string>& args);
std::string handleDCR(KVStore& kvstore, const std::vector<std::string>& args);
std::string handleEXPIRE(KVStore& kvstore, const std::vector<std::string>& args);
std::string handleTTL(KVStore& kvstore, const std::vector<std::string>& args);
std::string handleFLUSHALL(KVStore& kvstore, const std::vector<std::string>& args);
std::string handleMGET(KVStore& kvstore, const std::vector<std::string>& args);

//List commands
std::string handleLPUSH(KVStore& kvstore, const std::vector<std::string>& args);
std::string handleRPUSH(KVStore& kvstore, const std::vector<std::string>& args);
std::string handleLPOP(KVStore& kvstore, const std::vector<std::string>& args);
std::string handleRPOP(KVStore& kvstore, const std::vector<std::string>& args);
std::string handleLRANGE(KVStore& kvstore, const std::vector<std::string>& args);
std::string handleLLEN(KVStore& kvstore, const std::vector<std::string>& args);
std::string handleLINDEX(KVStore& kvstore, const std::vector<std::string>& args);
std::string handleLSET(KVStore& kvstore, const std::vector<std::string>& args);
std::string handleLREM(KVStore& kvstore, const std::vector<std::string>& args);

//Set commands
std::string handleSADD(KVStore& kvstore, const std::vector<std::string>& args);
std::string handleSREM(KVStore& kvstore, const std::vector<std::string>& args);
std::string handleSISMEMBER(KVStore& kvstore, const std::vector<std::string>& args);
std::string handleSMEMBERS(KVStore& kvstore, const std::vector<std::string>& args);
std::string handleSCARD(KVStore& kvstore, const std::vector<std::string>& args);
std::string handleSPOP(KVStore& kvstore, const std::vector<std::string>& args);

//Hash commands
std::string handleHSET(KVStore& kvstore, const std::vector<std::string>& args);
std::string handleHGET(KVStore& kvstore, const std::vector<std::string>& args);
std::string handleHDEL(KVStore& kvstore, const std::vector<std::string>& args);
std::string handleHEXISTS(KVStore& kvstore, const std::vector<std::string>& args);
std::string handleHLEN(KVStore& kvstore, const std::vector<std::string>& args);
std::string handleHKEYS(KVStore& kvstore, const std::vector<std::string>& args);
std::string handleHVALS(KVStore& kvstore, const std::vector<std::string>& args);
std::string handleHMGET(KVStore& kvstore, const std::vector<std::string>& args);

//Transaction commands
std::string handleMULTI(KVStore& kvstore, const std::vector<std::string>& args);
std::string handleEXEC(KVStore& kvstore, const std::vector<std::string>& args);
std::string handleDISCARD(KVStore& kvstore, const std::vector<std::string>& args);
std::string handleWATCH(KVStore& kvstore, const std::vector<std::string>& args);

//Pub/Sub commands
std::string handlePUBLISH(KVStore& kvstore, const std::vector<std::string>& args);
std::string handleSUBSCRIBE(KVStore& kvstore, const std::vector<std::string>& args);
std::string handleUNSUBSCRIBE(KVStore& kvstore, const std::vector<std::string>& args);

