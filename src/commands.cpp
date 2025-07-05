#include <vector>
#include <string>
#include <format>
#include <iostream>

#include "commands.hpp"
#include "kvstore.hpp"
#include "util.hpp"

Commands strToCmd(const std::string& cmd)
{
    if (cmd == "PING") return Commands::PING;
    if (cmd == "ECHO") return Commands::ECHO;
    if (cmd == "SET") return Commands::SET;
    if (cmd == "GET") return Commands::GET;
    if (cmd == "DEL") return Commands::DEL;
    if (cmd == "EXISTS") return Commands::EXISTS;
    if (cmd == "INCR") return Commands::INCR;
    if (cmd == "DCR") return Commands::DCR;
    if (cmd == "EXPIRE") return Commands::EXPIRE;
    if (cmd == "TTL") return Commands::TTL;
    if (cmd == "FLUSHALL") return Commands::FLUSHALL;
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

    return Commands::UNKNOWN;
}

//Basic commands
std::string handlePING(const std::vector<std::string>& args)
{
    if (args.size() > 1)
    {
        std::cerr << "Command arguments malformed" << std::endl;
        return argumentError("1 or none", args.size());
    }

    return args.size() == 1 ? std::format("${}\r\n{}\r\n", args[0].length(), args[0]) : "+PONG\r\n";
}
std::string handleECHO(const std::vector<std::string>& args)
{
    if (args.size() != 1)
    {
        std::cerr << "Command arguments malformed" << std::endl;
        return argumentError("1", args.size());
    }
    return std::format("${}\r\n{}\r\n", args[0].length(), args[0]);
}
std::string handleSET(KVStore& kvstore, const std::vector<std::string>& args)
{
    if (args.size() != 2)
    {
        std::cerr << "Command arguments malformed" << std::endl;
        return argumentError("2", args.size());
    }

    return kvstore.set(args[0], args[1]) ? "+OK\r\n" : "-ERR something went wrong in set\r\n";
}
std::string handleGET(KVStore& kvstore, const std::vector<std::string>& args)
{
    if (args.size() != 1)
    {
        std::cerr << "Command arguments malformed" << std::endl;
        return argumentError("1", args.size());
    }
    if (auto err = checkTypeError(kvstore, args[0], storeType::STR)) return *err;

    auto val = kvstore.get(args[0]);
    return val ? std::format("${}\r\n{}\r\n", val->length(), val.value()) : "$-1\r\n";
}
std::string handleDEL(KVStore& kvstore, const std::vector<std::string>& args)
{
    if (args.empty())
    {
        std::cerr << "Command arguments malformed" << std::endl;
        return argumentError("1 or more", args.size());
    }

    return std::format(":{}\r\n",kvstore.del(args));
}
std::string handleEXISTS(KVStore& kvstore, const std::vector<std::string>& args)
{
    if (args.empty())
    {
        std::cerr << "Command arguments malformed" << std::endl;
        return argumentError("1 or more", args.size());
    }

    return std::format(":{}\r\n", kvstore.exists(args));
}
std::string handleINCR(KVStore& kvstore, const std::vector<std::string>& args)
{
    if (args.size() != 1)
    {
        std::cerr << "Command arguments malformed" << std::endl;
        return argumentError("1", args.size());
    }

    if (auto found = kvstore.incr(args[0])) return std::format(":{}\r\n", found.value());
    return "-ERR value is not number or out of range\r\n";
}
std::string handleDCR(KVStore& kvstore, const std::vector<std::string>& args)
{
    if (args.size() != 1)
    {
        std::cerr << "Command arguments malformed" << std::endl;
        return argumentError("1", args.size());
    }

    if (auto found = kvstore.dcr(args[0])) return std::format(":{}\r\n", found.value());
    return "-ERR value is not number or out of range\r\n";
}
std::string handleEXPIRE(KVStore& kvstore, const std::vector<std::string>& args)
{
    if (args.size() != 2)
    {
        std::cerr << "Command arguments malformed" << std::endl;
        return argumentError("2", args.size());
    }

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
    if (args.size() != 1)
    {
        std::cerr << "Command arguments malformed" << std::endl;
        return argumentError("1", args.size());
    }
    return std::format(":{}\r\n", kvstore.ttl(args[0]));
}
std::string handleFLUSHALL(KVStore& kvstore, const std::vector<std::string>& args)
{
    if (!args.empty())
    {
        std::cerr << "Command arguments malformed" << std::endl;
        return argumentError("0", args.size());
    }
    kvstore.flushall();
    return "+OK\r\n";
}
std::string handleMGET(KVStore& kvstore, const std::vector<std::string>& args)
{
    if (args.empty())
    {
        std::cerr << "Command arguments malformed" << std::endl;
        return argumentError("1 or more", args.size());
    }

    const auto vals = kvstore.mget(args);
    std::string resp = std::format("*{}\r\n", vals.size());
    for (const auto& i : vals)
    {
        if (i == std::nullopt) resp += std::format("$-1\r\n");
        else resp += std::format("${}\r\n{}\r\n", i->length(), *i);

    }
    return resp;
}

//List commands
std::string handleLPUSH(KVStore& kvstore, const std::vector<std::string>& args)
{
    if (args.empty())
    {
        std::cerr << "Command arguments malformed" << std::endl;
        return argumentError("1 or more", args.size());
    }
    if (auto err = checkTypeError(kvstore, args[0], storeType::LIST)) return *err;

    return std::format(":{}\r\n",kvstore.lpush(args));
}
std::string handleRPUSH(KVStore& kvstore, const std::vector<std::string>& args)
{
    if (args.empty())
    {
        std::cerr << "Command arguments malformed" << std::endl;
        return argumentError("1 or more", args.size());
    }
    if (auto err = checkTypeError(kvstore, args[0], storeType::LIST)) return *err;

    return std::format(":{}\r\n",kvstore.rpush(args));
}
std::string handleLPOP(KVStore& kvstore, const std::vector<std::string>& args)
{
    if (args.size() != 1)
    {
        std::cerr << "Command arguments malformed" << std::endl;
        return argumentError("1", args.size());
    }
    if (auto err = checkTypeError(kvstore, args[0], storeType::LIST)) return *err;

    auto val = kvstore.lpop(args[0]);
    return val ? std::format("${}\r\n{}\r\n", val->length(), val.value()) : "$-1\r\n";
}
std::string handleRPOP(KVStore& kvstore, const std::vector<std::string>& args)
{
    if (args.size() != 1)
    {
        std::cerr << "Command arguments malformed" << std::endl;
        return argumentError("1", args.size());
    }
    if (auto err = checkTypeError(kvstore, args[0], storeType::LIST)) return *err;

    auto val = kvstore.rpop(args[0]);
    return val ? std::format("${}\r\n{}\r\n", val->length(), val.value()) : "$-1\r\n";
}
std::string handleLRANGE(KVStore& kvstore, const std::vector<std::string>& args)
{
    if (args.size() != 3)
    {
        std::cerr << "Command arguments malformed" << std::endl;
        return argumentError("3", args.size());
    }
    if (auto err = checkTypeError(kvstore, args[0], storeType::LIST)) return *err;

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
    if (args.size() != 1)
    {
        std::cerr << "Command arguments malformed" << std::endl;
        return argumentError("1", args.size());
    }
    if (auto err = checkTypeError(kvstore, args[0], storeType::LIST)) return *err;

    return std::format(":{}\r\n", kvstore.llen(args[0]));
}
std::string handleLINDEX(KVStore& kvstore, const std::vector<std::string>& args)
{
    if (args.size() != 2)
    {
        std::cerr << "Command arguments malformed" << std::endl;
        return argumentError("2", args.size());
    }
    if (auto err = checkTypeError(kvstore, args[0], storeType::LIST)) return *err;

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
    if (args.size() != 3)
    {
        std::cerr << "Command arguments malformed" << std::endl;
        return argumentError("3", args.size());
    }
    if (auto err = checkTypeError(kvstore, args[0], storeType::LIST)) return *err;

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
    if (args.size() != 3)
    {
        std::cerr << "Command arguments malformed" << std::endl;
        return argumentError("3", args.size());
    }
    if (auto err = checkTypeError(kvstore, args[0], storeType::LIST)) return *err;

    try
    {
        const int count = std::stoi(args[1]);
        return std::format(":{}\r\n", kvstore.lrem(args[0], count, args[2]));
    }
    catch (const std::exception&) {
        return "-ERR value is not an integer or out of range\r\n";
    }
}

//Set commands
std::string handleSADD(KVStore& kvstore, const std::vector<std::string>& args)
{
    if (args.size() < 2)
    {
        std::cerr << "Command arguments malformed" << std::endl;
        return argumentError("2 or more", args.size());
    }
    if (auto err = checkTypeError(kvstore, args[0], storeType::SET)) return *err;

    return std::format(":{}\r\n", kvstore.sadd(args));
}
std::string handleSREM(KVStore& kvstore, const std::vector<std::string>& args)
{
    if (args.size() < 2)
    {
        std::cerr << "Command arguments malformed" << std::endl;
        return argumentError("2 or more", args.size());
    }
    if (auto err = checkTypeError(kvstore, args[0], storeType::SET)) return *err;

    return std::format(":{}\r\n", kvstore.srem(args));
}
std::string handleSISMEMBER(KVStore& kvstore, const std::vector<std::string>& args)
{
    if (args.size() != 2)
    {
        std::cerr << "Command arguments malformed" << std::endl;
        return argumentError("2", args.size());
    }
    if (auto err = checkTypeError(kvstore, args[0], storeType::SET)) return *err;

    return kvstore.sismember(args[0], args[1]) ? ":1\r\n" : ":0\r\n";
}
std::string handleSMEMBERS(KVStore& kvstore, const std::vector<std::string>& args)
{
    if (args.size() != 1)
    {
        std::cerr << "Command arguments malformed" << std::endl;
        return argumentError("1", args.size());
    }
    if (auto err = checkTypeError(kvstore, args[0], storeType::SET)) return *err;

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
    if (args.size() != 1)
    {
        std::cerr << "Command arguments malformed" << std::endl;
        return argumentError("1", args.size());
    }
    if (auto err = checkTypeError(kvstore, args[0], storeType::SET)) return *err;

    return std::format(":{}\r\n", kvstore.scard(args[0]));
}
std::string handleSPOP(KVStore& kvstore, const std::vector<std::string>& args)
{
    if (!(args.size() == 1 || args.size() == 2))
    {
        std::cerr << "Command arguments malformed" << std::endl;
        return argumentError("1 or 2", args.size());
    }
    if (auto err = checkTypeError(kvstore, args[0], storeType::SET)) return *err;

    try
    {
        int count;
        std::string resp = "";
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

//Hash commands
std::string handleHSET(KVStore& kvstore, const std::vector<std::string>& args)
{
    if (args.size() < 3)
    {
        std::cerr << "Command arguments malformed" << std::endl;
        return argumentError("3 or more", args.size());
    }
    if (args.size() % 2 == 0)
    {
        std::cerr << "Command arguments malformed" << std::endl;
        return "-ERR expected pair of fields and values";
    }
    if (auto err = checkTypeError(kvstore, args[0], storeType::HASH)) return *err;

    return std::format(":{}\r\n", kvstore.hset(args));
}
std::string handleHGET(KVStore& kvstore, const std::vector<std::string>& args)
{
    if (args.size() != 2)
    {
        std::cerr << "Command arguments malformed" << std::endl;
        return argumentError("2", args.size());
    }
    if (auto err = checkTypeError(kvstore, args[0], storeType::HASH)) return *err;

    auto val = kvstore.hget(args[0], args[1]);
    return val ? std::format("${}\r\n{}\r\n", val->length(), val.value()) : "$-1\r\n";
}
std::string handleHDEL(KVStore& kvstore, const std::vector<std::string>& args)
{
    if (args.size() < 2)
    {
        std::cerr << "Command arguments malformed" << std::endl;
        return argumentError("2 or more", args.size());
    }
    if (auto err = checkTypeError(kvstore, args[0], storeType::HASH)) return *err;

    return std::format(":{}\r\n", kvstore.hdel(args));
}
std::string handleHEXISTS(KVStore& kvstore, const std::vector<std::string>& args)
{
    if (args.size() != 2)
    {
        std::cerr << "Command arguments malformed" << std::endl;
        return argumentError("2", args.size());
    }
    if (auto err = checkTypeError(kvstore, args[0], storeType::HASH)) return *err;

    return kvstore.hexists(args[0], args[1]) ? ":1\r\n" : ":0\r\n";
}
std::string handleHLEN(KVStore& kvstore, const std::vector<std::string>& args)
{
    if (args.size() != 1)
    {
        std::cerr << "Command arguments malformed" << std::endl;
        return argumentError("1", args.size());
    }
    if (auto err = checkTypeError(kvstore, args[0], storeType::HASH)) return *err;

    return std::format(":{}\r\n", kvstore.hlen(args[0]));
}
std::string handleHKEYS(KVStore& kvstore, const std::vector<std::string>& args)
{
    if (args.size() != 1)
    {
        std::cerr << "Command arguments malformed" << std::endl;
        return argumentError("1", args.size());
    }
    if (auto err = checkTypeError(kvstore, args[0], storeType::HASH)) return *err;

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
    if (args.size() != 1)
    {
        std::cerr << "Command arguments malformed" << std::endl;
        return argumentError("1", args.size());
    }
    if (auto err = checkTypeError(kvstore, args[0], storeType::HASH)) return *err;

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
    if (args.size() < 2)
    {
        std::cerr << "Command arguments malformed" << std::endl;
        return argumentError("2 or more", args.size());
    }
    if (auto err = checkTypeError(kvstore, args[0], storeType::HASH)) return *err;

    const auto vals = kvstore.hmget(args);
    std::string resp = std::format("*{}\r\n", vals.size());
    for (const auto& i : vals)
    {
        if (i == std::nullopt) resp += std::format("$-1\r\n");
        else resp += std::format("${}\r\n{}\r\n", i->length(), *i);
    }
    return resp;
}

//TODO pub/sub
//TODO advanced data structures and commands (lists,sets,hashs,sortedset)
//TODO multi exec discard transactions
//TODO threadpool
//
//Later do lru and clean error handling
