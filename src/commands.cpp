#include <vector>
#include <string>
#include <format>
#include <iostream>

#include "commands.hpp"
#include "kvstore.hpp"
#include "pubsub.hpp"
#include "session.hpp"
#include "util.hpp"

Commands strToCmd(const std::string& cmd)
{
    if (cmd == "PING") return Commands::PING;
    if (cmd == "ECHO") return Commands::ECHO;
    if (cmd == "DEL") return Commands::DEL;
    if (cmd == "EXISTS") return Commands::EXISTS;
    if (cmd == "FLUSHALL") return Commands::FLUSHALL;
    if (cmd == "SET") return Commands::SET;
    if (cmd == "GET") return Commands::GET;
    if (cmd == "INCR") return Commands::INCR;
    if (cmd == "DCR") return Commands::DCR;
    if (cmd == "INCRBY") return Commands::INCRBY;
    if (cmd == "DCRBY") return Commands::DCRBY;
    if (cmd == "APPEND") return Commands::APPEND;
    if (cmd == "EXPIRE") return Commands::EXPIRE;
    if (cmd == "TTL") return Commands::TTL;
    if (cmd == "PERSIST") return Commands::PERSIST;
    if (cmd == "MGET") return Commands::MGET;
    if (cmd == "LPUSH") return Commands::LPUSH;
    if (cmd == "RPUSH") return Commands::RPUSH;
    if (cmd == "LPOP") return Commands::LPOP;
    if (cmd == "RPOP") return Commands::RPOP;
    if (cmd == "LRANGE") return Commands::LRANGE;
    if (cmd == "LLEN") return Commands::LLEN;
    if (cmd == "LINDEX") return Commands::LINDEX;
    if (cmd == "LSET") return Commands::LSET;
    if (cmd == "LREM") return Commands::LREM;
    if (cmd == "SADD") return Commands::SADD;
    if (cmd == "SREM") return Commands::SREM;
    if (cmd == "SISMEMBER") return Commands::SISMEMBER;
    if (cmd == "SMEMBERS") return Commands::SMEMBERS;
    if (cmd == "SCARD") return Commands::SCARD;
    if (cmd == "SPOP") return Commands::SPOP;
    if (cmd == "HSET") return Commands::HSET;
    if (cmd == "HGET") return Commands::HGET;
    if (cmd == "HDEL") return Commands::HDEL;
    if (cmd == "HEXISTS") return Commands::HEXISTS;
    if (cmd == "HLEN") return Commands::HLEN;
    if (cmd == "HKEYS") return Commands::HKEYS;
    if (cmd == "HVALS") return Commands::HVALS;
    if (cmd == "HMGET") return Commands::HMGET;
    if (cmd == "HGETALL") return Commands::HGETALL;
    if (cmd == "MULTI") return Commands::MULTI;
    if (cmd == "EXEC") return Commands::EXEC;
    if (cmd == "DISCARD") return Commands::DISCARD;
    if (cmd == "PUBLISH") return Commands::PUBLISH;
    if (cmd == "SUBSCRIBE") return Commands::SUBSCRIBE;
    if (cmd == "UNSUBSCRIBE") return Commands::UNSUBSCRIBE;
    if (cmd == "CONFIG") return Commands::CONFIG;
    if (cmd == "TYPE") return Commands::TYPE;
    if (cmd == "SAVE") return Commands::SAVE;
    return Commands::UNKNOWN;
}

std::string handlePING(const std::vector<std::string>& args)
{
    if (args.size() > 1) return argumentError("1 or none", args.size());

    return args.size() == 1 ? std::format("${}\r\n{}\r\n", args[0].length(), args[0]) : "+PONG\r\n";
}
std::string handleECHO(const std::vector<std::string>& args)
{
    if (args.size() != 1) return argumentError("1", args.size());

    return std::format("${}\r\n{}\r\n", args[0].length(), args[0]);
}
std::string handleDEL(KVStore& kvstore, const std::vector<std::string>& args)
{
    if (args.empty()) return argumentError("1 or more", args.size());

    return std::format(":{}\r\n",kvstore.del(args));
}
std::string handleEXISTS(KVStore& kvstore, const std::vector<std::string>& args)
{
    if (args.empty()) return argumentError("1 or more", args.size());

    return std::format(":{}\r\n", kvstore.exists(args));
}
std::string handleFLUSHALL(KVStore& kvstore, const std::vector<std::string>& args)
{
    if (!args.empty()) return argumentError("0", args.size());

    kvstore.flushall();
    return "+OK\r\n";
}

std::string handleSET(KVStore& kvstore, const std::vector<std::string>& args)
{
    if (args.size() != 2) return argumentError("2", args.size());
    if (auto err = kvstore.checkTypeError(args[0], storeType::STR)) return *err;

    return kvstore.set(args[0], args[1]) ? "+OK\r\n" : "-ERR something went wrong in set\r\n";
}
std::string handleGET(KVStore& kvstore, const std::vector<std::string>& args)
{
    if (args.size() != 1) return argumentError("1", args.size());
    if (auto err = kvstore.checkTypeError(args[0], storeType::STR)) return *err;

    auto val = kvstore.get(args[0]);
    return val ? std::format("${}\r\n{}\r\n", val->length(), val.value()) : "$-1\r\n";
}
std::string handleINCR(KVStore& kvstore, const std::vector<std::string>& args)
{
    if (args.size() != 1) return argumentError("1", args.size());
    if (auto err = kvstore.checkTypeError(args[0], storeType::STR)) return *err;

    if (auto found = kvstore.incr(args[0])) return std::format(":{}\r\n", found.value());
    return "-ERR value is not number or out of range\r\n";
}
std::string handleDCR(KVStore& kvstore, const std::vector<std::string>& args)
{
    if (args.size() != 1) return argumentError("1", args.size());
    if (auto err = kvstore.checkTypeError(args[0], storeType::STR)) return *err;

    if (auto found = kvstore.dcr(args[0])) return std::format(":{}\r\n", found.value());
    return "-ERR value is not number or out of range\r\n";
}
std::string handleINCRBY(KVStore& kvstore, const std::vector<std::string>& args)
{
    if (args.size() != 2) return argumentError("2", args.size());
    if (auto err = kvstore.checkTypeError(args[0], storeType::STR)) return *err;

    try
    {
        const int count = std::stoi(args[1]);
        if (auto found = kvstore.incrby(args[0], count)) return std::format(":{}\r\n", found.value());
        return "-ERR value is not number or out of range\r\n";
    }
    catch (const std::exception&) {
        return "-ERR arg given not a number\r\n";
    }
}
std::string handleDCRBY(KVStore& kvstore, const std::vector<std::string>& args)
{
    if (args.size() != 2) return argumentError("2", args.size());
    if (auto err = kvstore.checkTypeError(args[0], storeType::STR)) return *err;

    try
    {
        const int count = std::stoi(args[1]);
        if (auto found = kvstore.dcrby(args[0], count)) return std::format(":{}\r\n", found.value());
        return "-ERR value is not number or out of range\r\n";
    }
    catch (const std::exception&) {
        return "-ERR arg given not a number\r\n";
    }
}
std::string handleMGET(KVStore& kvstore, const std::vector<std::string>& args)
{
    if (args.empty()) return argumentError("1 or more", args.size());


    const auto vals = kvstore.mget(args);
    std::string resp = std::format("*{}\r\n", vals.size());
    for (const auto& i : vals)
    {
        if (i == std::nullopt) resp += std::format("$-1\r\n");
        else resp += std::format("${}\r\n{}\r\n", i->length(), *i);
    }
    return resp;
}
std::string handleAPPEND(KVStore& kvstore, const std::vector<std::string>& args)
{
    if (args.size() != 2) return argumentError("2", args.size());
    if (auto err = kvstore.checkTypeError(args[0], storeType::STR)) return *err;

    return std::format(":{}\r\n", kvstore.append(args[0], args[1]));
}

std::string handleEXPIRE(KVStore& kvstore, const std::vector<std::string>& args)
{
    if (args.size() != 2) return argumentError("2", args.size());

    try
    {
        if (const int seconds = std::stoi(args[1]); kvstore.expire(args[0], seconds)) return ":1\r\n";
        return ":0\r\n";
    }
    catch (const std::exception&) {
        return "-ERR seconds provided not number\r\n";
    }
}
std::string handleTTL(KVStore& kvstore, const std::vector<std::string>& args)
{
    if (args.size() != 1) return argumentError("1", args.size());

    return std::format(":{}\r\n", kvstore.ttl(args[0]));
}
std::string handlePERSIST(KVStore& kvstore, const std::vector<std::string>& args)
{
    if (args.size() != 1) return argumentError("1", args.size());

    return kvstore.persist(args[0]) ? ":1\r\n" : ":0\r\n";
}

std::string handleLPUSH(KVStore& kvstore, const std::vector<std::string>& args)
{
    if (args.empty()) return argumentError("1 or more", args.size());
    if (auto err = kvstore.checkTypeError(args[0], storeType::LIST)) return *err;

    return std::format(":{}\r\n",kvstore.lpush(args));
}
std::string handleRPUSH(KVStore& kvstore, const std::vector<std::string>& args)
{
    if (args.empty()) return argumentError("1 or more", args.size());
    if (auto err = kvstore.checkTypeError(args[0], storeType::LIST)) return *err;

    return std::format(":{}\r\n",kvstore.rpush(args));
}
std::string handleLPOP(KVStore& kvstore, const std::vector<std::string>& args)
{
    if (args.size() != 1) return argumentError("1", args.size());
    if (auto err = kvstore.checkTypeError(args[0], storeType::LIST)) return *err;

    auto val = kvstore.lpop(args[0]);
    return val ? std::format("${}\r\n{}\r\n", val->length(), val.value()) : "$-1\r\n";
}
std::string handleRPOP(KVStore& kvstore, const std::vector<std::string>& args)
{
    if (args.size() != 1) return argumentError("1", args.size());
    if (auto err = kvstore.checkTypeError(args[0], storeType::LIST)) return *err;

    auto val = kvstore.rpop(args[0]);
    return val ? std::format("${}\r\n{}\r\n", val->length(), val.value()) : "$-1\r\n";
}
std::string handleLRANGE(KVStore& kvstore, const std::vector<std::string>& args)
{
    if (args.size() != 3) return argumentError("3", args.size());
    if (auto err = kvstore.checkTypeError(args[0], storeType::LIST)) return *err;

    try
    {
        const int start = std::stoi(args[1]);
        const int stop = std::stoi(args[2]);
        const auto vals = kvstore.lrange(args[0], start, stop);
        std::string resp = std::format("*{}\r\n", vals.size());
        for (const auto& i : vals)
        {
            if (i == std::nullopt) resp += std::format("$-1\r\n");
            else resp += std::format("${}\r\n{}\r\n", i->length(), *i);
        }
        return resp;
    }
    catch (const std::exception&) {
        return "-ERR value is not an integer or out of range\r\n";
    }
}
std::string handleLLEN(KVStore& kvstore, const std::vector<std::string>& args)
{
    if (args.size() != 1) return argumentError("1", args.size());
    if (auto err = kvstore.checkTypeError(args[0], storeType::LIST)) return *err;

    return std::format(":{}\r\n", kvstore.llen(args[0]));
}
std::string handleLINDEX(KVStore& kvstore, const std::vector<std::string>& args)
{
    if (args.size() != 2) return argumentError("2", args.size());
    if (auto err = kvstore.checkTypeError(args[0], storeType::LIST)) return *err;

    try
    {
        const int index = std::stoi(args[1]);
        auto val = kvstore.lindex(args[0], index);
        return val ? std::format("${}\r\n{}\r\n", val->length(), val.value()) : "$-1\r\n";
    }
    catch (const std::exception&) {
        return "-ERR value is not an integer or out of range\r\n";
    }
}
std::string handleLSET(KVStore& kvstore, const std::vector<std::string>& args)
{
    if (args.size() != 3) return argumentError("3", args.size());
    if (auto err = kvstore.checkTypeError(args[0], storeType::LIST)) return *err;

    try
    {
        const int index = std::stoi(args[1]);
        return kvstore.lset(args[0], index, args[2]) ? "+OK\r\n" : "-ERR no such key or value out of range\r\n";  //fix ERR to make sense
    }
    catch (const std::exception&) {
        return "-ERR value is not an integer or out of range\r\n";
    }
}
std::string handleLREM(KVStore& kvstore, const std::vector<std::string>& args)
{
    if (args.size() != 3) return argumentError("3", args.size());
    if (auto err = kvstore.checkTypeError(args[0], storeType::LIST)) return *err;

    try
    {
        const int count = std::stoi(args[1]);
        return std::format(":{}\r\n", kvstore.lrem(args[0], count, args[2]));
    }
    catch (const std::exception&) {
        return "-ERR value is not an integer or out of range\r\n";
    }
}

std::string handleSADD(KVStore& kvstore, const std::vector<std::string>& args)
{
    if (args.size() < 2) return argumentError("2 or more", args.size());
    if (auto err = kvstore.checkTypeError(args[0], storeType::SET)) return *err;

    return std::format(":{}\r\n", kvstore.sadd(args));
}
std::string handleSREM(KVStore& kvstore, const std::vector<std::string>& args)
{
    if (args.size() < 2) return argumentError("2 or more", args.size());
    if (auto err = kvstore.checkTypeError(args[0], storeType::SET)) return *err;

    return std::format(":{}\r\n", kvstore.srem(args));
}
std::string handleSISMEMBER(KVStore& kvstore, const std::vector<std::string>& args)
{
    if (args.size() != 2) return argumentError("2", args.size());
    if (auto err = kvstore.checkTypeError(args[0], storeType::SET)) return *err;

    return kvstore.sismember(args[0], args[1]) ? ":1\r\n" : ":0\r\n";
}
std::string handleSMEMBERS(KVStore& kvstore, const std::vector<std::string>& args)
{
    if (args.size() != 1) return argumentError("1", args.size());
    if (auto err = kvstore.checkTypeError(args[0], storeType::SET)) return *err;

    const auto vals = kvstore.smembers(args[0]);
    std::string resp = std::format("*{}\r\n", vals.size());
    for (const auto& i : vals)
    {
        if (i == std::nullopt) resp += std::format("$-1\r\n");
        else resp += std::format("${}\r\n{}\r\n", i->length(), *i);
    }
    return resp;
}
std::string handleSCARD(KVStore& kvstore, const std::vector<std::string>& args)
{
    if (args.size() != 1) return argumentError("1", args.size());
    if (auto err = kvstore.checkTypeError(args[0], storeType::SET)) return *err;

    return std::format(":{}\r\n", kvstore.scard(args[0]));
}
std::string handleSPOP(KVStore& kvstore, const std::vector<std::string>& args)
{
    if (!(args.size() == 1 || args.size() == 2)) return argumentError("1 or 2", args.size());
    if (auto err = kvstore.checkTypeError(args[0], storeType::SET)) return *err;

    try
    {
        int count;
        std::string resp;
        if (args.size() == 2) count = std::stoi(args[1]);
        else count = 1;

        const auto vals = kvstore.spop(args[0], count);
        if (vals.empty()) return "$-1\r\n";

        if (vals.size() > 1) resp += std::format("*{}\r\n", vals.size());
        for (const auto& i : vals)
        {
            if (i == std::nullopt) resp += std::format("$-1\r\n");
            else resp += std::format("${}\r\n{}\r\n", i->length(), *i);
        }
        return resp;
    }
    catch (const std::exception&) {
        return "-ERR value is not an integer or out of range\r\n";
    }
}

std::string handleHSET(KVStore& kvstore, const std::vector<std::string>& args)
{
    if (args.size() < 3) return argumentError("3 or more", args.size());
    if (args.size() % 2 == 0) return "-ERR expected pair of fields and values";
    if (auto err = kvstore.checkTypeError(args[0], storeType::HASH)) return *err;

    return std::format(":{}\r\n", kvstore.hset(args));
}
std::string handleHGET(KVStore& kvstore, const std::vector<std::string>& args)
{
    if (args.size() != 2) return argumentError("2", args.size());
    if (auto err = kvstore.checkTypeError(args[0], storeType::HASH)) return *err;

    auto val = kvstore.hget(args[0], args[1]);
    return val ? std::format("${}\r\n{}\r\n", val->length(), val.value()) : "$-1\r\n";
}
std::string handleHDEL(KVStore& kvstore, const std::vector<std::string>& args)
{
    if (args.size() < 2) return argumentError("2 or more", args.size());
    if (auto err = kvstore.checkTypeError(args[0], storeType::HASH)) return *err;

    return std::format(":{}\r\n", kvstore.hdel(args));
}
std::string handleHEXISTS(KVStore& kvstore, const std::vector<std::string>& args)
{
    if (args.size() != 2) return argumentError("2", args.size());
    if (auto err = kvstore.checkTypeError(args[0], storeType::HASH)) return *err;

    return kvstore.hexists(args[0], args[1]) ? ":1\r\n" : ":0\r\n";
}
std::string handleHLEN(KVStore& kvstore, const std::vector<std::string>& args)
{
    if (args.size() != 1) return argumentError("1", args.size());
    if (auto err = kvstore.checkTypeError(args[0], storeType::HASH)) return *err;

    return std::format(":{}\r\n", kvstore.hlen(args[0]));
}
std::string handleHKEYS(KVStore& kvstore, const std::vector<std::string>& args)
{
    if (args.size() != 1) return argumentError("1", args.size());
    if (auto err = kvstore.checkTypeError(args[0], storeType::HASH)) return *err;

    const auto vals = kvstore.hkeys(args[0]);
    std::string resp = std::format("*{}\r\n", vals.size());
    for (const auto& i : vals)
    {
        if (i == std::nullopt) resp += std::format("$-1\r\n");
        else resp += std::format("${}\r\n{}\r\n", i->length(), *i);
    }
    return resp;
}
std::string handleHVALS(KVStore& kvstore, const std::vector<std::string>& args)
{
    if (args.size() != 1) return argumentError("1", args.size());
    if (auto err = kvstore.checkTypeError(args[0], storeType::HASH)) return *err;

    const auto vals = kvstore.hvals(args[0]);
    std::string resp = std::format("*{}\r\n", vals.size());
    for (const auto& i : vals)
    {
        if (i == std::nullopt) resp += std::format("$-1\r\n");
        else resp += std::format("${}\r\n{}\r\n", i->length(), *i);
    }
    return resp;
}
std::string handleHMGET(KVStore& kvstore, const std::vector<std::string>& args)
{
    if (args.size() < 2) return argumentError("2 or more", args.size());
    if (auto err = kvstore.checkTypeError(args[0], storeType::HASH)) return *err;

    const auto vals = kvstore.hmget(args);
    std::string resp = std::format("*{}\r\n", vals.size());
    for (const auto& i : vals)
    {
        if (i == std::nullopt) resp += std::format("$-1\r\n");
        else resp += std::format("${}\r\n{}\r\n", i->length(), *i);
    }
    return resp;
}
std::string handleHGETALL(KVStore& kvstore, const std::vector<std::string>& args)
{
    if (args.size() != 1) return argumentError("1", args.size());
    if (auto err = kvstore.checkTypeError(args[0], storeType::HASH)) return *err;

    const auto vals = kvstore.hgetall(args[0]);
    std::string resp = std::format("*{}\r\n", vals.size());
    for (const auto& i : vals)
    {
        if (i == std::nullopt) resp += std::format("$-1\r\n");
        else resp += std::format("${}\r\n{}\r\n", i->length(), *i);
    }
    return resp;

}

std::string handlePUBLISH(PubSub& ps, const std::vector<std::string>& args)
{
    if (args.size() != 2) return argumentError("2", args.size());
    return std::format(":{}\r\n", ps.publish(args[0],args[1]));
}
std::string handleSUBSCRIBE(PubSub& ps, const std::vector<std::string>& args, const int sock)
{
    if (args.empty()) return argumentError("1 or more", args.size());

    std::string resp;
    for (const auto& i : args)
    {
        resp += std::format("*3\r\n$9\r\nsubscribe\r\n${}\r\n{}\r\n:{}\r\n", i.size(), i, ps.subscribe(i, sock));
    }
    return resp;
}
std::string handleUNSUBSCRIBE(PubSub& ps, const std::vector<std::string>& args, const int sock)
{
    if (args.empty()) return argumentError("1 or more", args.size());

    std::string resp;
    for (const auto& i : args)
    {
        resp += std::format("*3\r\n$11\r\nunsubscribe\r\n${}\r\n{}\r\n:{}\r\n", i.size(), i, ps.unsubscribe(i, sock));
    }
    return resp;
}

std::string handleMULTI(Session* session, const std::vector<std::string>& args)
{
    if (!args.empty()) return argumentError("0", args.size());
    if (session->transActive) return "-ERR MULTI calls can not be nested";
    session->transActive = true;
    session->transQueue.clear();
    return "+OK\r\n";
}
//EXEC handled in server.cpp handleCommands
std::string handleDISCARD(Session* session, const std::vector<std::string>& args)
{
    if (!args.empty()) return argumentError("0", args.size());
    if (session->transActive) return "-ERR DISCARD without MULTI";
    session->transActive = false;
    session->transQueue.clear();
    return "+OK\r\n";
}

std::string handleCONFIG(const std::vector<std::string>& args) //TODO remove or actually implement (only for benchmark start)
{
    if (args[0] == "GET") return "*14\r\n$7\r\ntimeout\r\n$1\r\n0\r\n$9\r\ndatabases\r\n$1\r\n1\r\n$11\r\nrequirepass\r\n$0\r\n\r\n$9\r\nmaxmemory\r\n$1\r\n0\r\n$3\r\ndir\r\n$5\r\n./data\r\n";
    return "-ERR not handled";
}
std::string handleTYPE(KVStore& kvstore, const std::vector<std::string>& args)
{
    if (args.size() != 1) return argumentError("1", args.size());
    if (const auto resp = kvstore.getType(args[0]))
    {
        switch (*resp)
        {
            case storeType::STR: return "+string\r\n";
            case storeType::LIST: return "+list\r\n";
            case storeType::SET: return "+set\r\n";
            case storeType::HASH: return "+hash\r\n";
            default:
                return "+none\r\n";
        }
    }
    return "+none\r\n"; // Fall back
}
std::string handleSAVE(KVStore& kvstore, const std::vector<std::string>& args)
{
    if (!args.empty()) return argumentError("0", args.size());
    kvstore.saveToDisk();
    return "+OK\r\n";
}