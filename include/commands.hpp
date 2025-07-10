#pragma once

#include <string>
#include "kvstore.hpp"
#include "pubsub.hpp"
#include "session.hpp"

enum class Commands
{
    PING, ECHO,
    DEL, EXISTS, FLUSHALL,
    SET, GET, INCR, DCR, INCRBY, DCRBY, MGET, APPEND,
    EXPIRE, TTL, PERSIST,
    LPUSH, RPUSH, LPOP, RPOP, LRANGE, LLEN, LINDEX, LSET, LREM,
    SADD, SREM, SISMEMBER, SMEMBERS, SCARD, SPOP,
    HSET, HGET, HDEL, HEXISTS, HLEN, HKEYS, HVALS, HMGET, HGETALL,
    PUBLISH, SUBSCRIBE, UNSUBSCRIBE,
    MULTI, EXEC, DISCARD,
    CONFIG, TYPE, SAVE,
    UNKNOWN
    //TODO INFO, BRPOP for task queues, LMOVE, KEYS, RENAME...
};

auto strToCmd(const std::string& cmd) -> Commands;

//Basic commands
std::string handlePING(const std::vector<std::string>& args);
std::string handleECHO(const std::vector<std::string>& args);
std::string handleDEL(KVStore& kvstore, const std::vector<std::string>& args);
std::string handleEXISTS(KVStore& kvstore, const std::vector<std::string>& args);
std::string handleFLUSHALL(KVStore& kvstore, const std::vector<std::string>& args);

//String commands
std::string handleSET(KVStore& kvstore, const std::vector<std::string>& args);
std::string handleGET(KVStore& kvstore, const std::vector<std::string>& args);
std::string handleINCR(KVStore& kvstore, const std::vector<std::string>& args);
std::string handleDCR(KVStore& kvstore, const std::vector<std::string>& args);
std::string handleINCRBY(KVStore& kvstore, const std::vector<std::string>& args);
std::string handleDCRBY(KVStore& kvstore, const std::vector<std::string>& args);
std::string handleMGET(KVStore& kvstore, const std::vector<std::string>& args);
std::string handleAPPEND(KVStore& kvstore, const std::vector<std::string>& args);

//TTL commands
std::string handleEXPIRE(KVStore& kvstore, const std::vector<std::string>& args);
std::string handleTTL(KVStore& kvstore, const std::vector<std::string>& args);
std::string handlePERSIST(KVStore& kvstore, const std::vector<std::string>& args);

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
std::string handleHGETALL(KVStore& kvstore, const std::vector<std::string>& args);

//Pub/Sub commands
std::string handlePUBLISH(PubSub& ps, const std::vector<std::string>& args);
std::string handleSUBSCRIBE(PubSub& ps, const std::vector<std::string>& args, int sock);
std::string handleUNSUBSCRIBE(PubSub& ps, const std::vector<std::string>& args, int sock);

//Transaction commands
std::string handleMULTI(Session* session, const std::vector<std::string>& args);
std::string handleEXEC(Session* session, const std::vector<std::string>& args);
std::string handleDISCARD(Session* session, const std::vector<std::string>& args);

// Misc commands
std::string handleCONFIG(const std::vector<std::string>& args);
std::string handleTYPE(KVStore& kvstore, const std::vector<std::string>& args);
std::string handleSAVE(KVStore& kvstore, const std::vector<std::string>& args);
