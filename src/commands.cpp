#include <vector>
#include <string>
#include <format>
#include <iostream>

#include <commands.hpp>
#include <kvstore.hpp>
#include <util.hpp>

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
    return Commands::UNKNOWN;
}

std::string handlePing(const std::vector<std::string>& args)
{
    if (args.size() > 1)
    {
        std::cerr << "Command arguments malformed" << std::endl;
        return argumentError("1 or none", args.size());
    }

    return args.size() == 1 ? std::format("${}\r\n{}\r\n", args[0].length(), args[0]) : "+PONG\r\n";
}
std::string handleEcho(const std::vector<std::string>& args)
{
    if (args.size() != 1)
    {
        std::cerr << "Command arguments malformed" << std::endl;
        return argumentError("1", args.size());
    }
    return std::format("${}\r\n{}\r\n", args[0].length(), args[0]);
}
std::string handleSet(KVStore& kvstore, const std::vector<std::string>& args)
{
    if (args.size() != 2)
    {
        std::cerr << "Command arguments malformed" << std::endl;
        return argumentError("2", args.size());
    }
    return kvstore.set(args[0], args[1]) ? "+OK\r\n" : "-ERR something went wrong in set\r\n";
}
std::string handleGet(KVStore& kvstore, const std::vector<std::string>& args)
{
    if (args.size() != 1)
    {
        std::cerr << "Command arguments malformed" << std::endl;
        return argumentError("1", args.size());
    }
    auto val = kvstore.get(args[0]);
    return val ? std::format("${}\r\n{}\r\n", val->length(), val.value()) : "$-1\r\n";
}
std::string handleDel(KVStore& kvstore, const std::vector<std::string>& args)
{
    if (args.size() < 1)
    {
        std::cerr << "Command arguments malformed" << std::endl;
        return argumentError("1 or more", args.size());
    }
    int deleted = kvstore.del(args);
    return std::format(":{}\r\n",deleted);
}
std::string handleExists(KVStore& kvstore, const std::vector<std::string>& args)
{
    if (args.size() < 1)
    {
        std::cerr << "Command arguments malformed" << std::endl;
        return argumentError("1 or more", args.size());
    }
    int found = kvstore.exists(args);
    return std::format(":{}\r\n", found);
}
std::string handleIncr(KVStore& kvstore, const std::vector<std::string>& args)
{
    if (args.size() != 1)
    {
        std::cerr << "Command arguments malformed" << std::endl;
        return argumentError("1", args.size());
    }
    auto found = kvstore.incr(args[0]);
    if (found) return std::format(":{}\r\n", found.value());
    return "-ERR value is not number or out of range\r\n";
}
std::string handleDcr(KVStore& kvstore, const std::vector<std::string>& args)
{
    if (args.size() != 1)
    {
        std::cerr << "Command arguments malformed" << std::endl;
        return argumentError("1", args.size());
    }
    auto found = kvstore.dcr(args[0]);
    if (found) return std::format(":{}\r\n", found.value());
    return "-ERR value is not number or out of range\r\n";
}
std::string handleExpire(KVStore& kvstore, const std::vector<std::string>& args)
{
    if (args.size() != 2)
    {
        std::cerr << "Command arguments malformed" << std::endl;
        return argumentError("2", args.size());
    }
    try
    {
        int seconds = std::stoi(args[1]);
        if (kvstore.expire(args[0], seconds)) return ":1\r\n";
        return ":0\r\n";
    } catch (const std::exception&) {
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
    int found = kvstore.ttl(args[0]);
    return std::format(":{}\r\n", found);
}
std::string handleFlushall(KVStore& kvstore, const std::vector<std::string>& args)
{
    if (args.size() > 0)
    {
        std::cerr << "Command arguments malformed" << std::endl;
        return argumentError("0", args.size());
    }
    kvstore.flushall();
    return "+OK\r\n";
}
std::string handleMget(KVStore& kvstore, const std::vector<std::string>& args)
{
    if (args.size() < 1)
    {
        std::cerr << "Command arguments malformed" << std::endl;
        return argumentError("1 or more", args.size());
    }
    auto vals = kvstore.mget(args);
    std::string resp = std::format("*{}\r\n", vals.size());
    for (const auto& i : vals)
    {
        if (i == std::nullopt) resp += std::format("$-1\r\n");
        else resp += std::format("${}\r\n{}\r\n", i->length(), *i);

    }
    return resp;
}

std::string handleLpush(KVStore& kvstore, const std::vector<std::string>& args)
{
    if (args.size() < 1)
    {
        std::cerr << "Command arguments malformed" << std::endl;
        return argumentError("1 or more", args.size());
    }
}

std::string handleRpush(KVStore& kvstore, const std::vector<std::string>& args)
{
    if (args.size() < 1)
    {
        std::cerr << "Command arguments malformed" << std::endl;
        return argumentError("1 or more", args.size());
    }
}

std::string handleLpop(KVStore& kvstore, const std::vector<std::string>& args)
{
    if (args.size() != 1)
    {
        std::cerr << "Command arguments malformed" << std::endl;
        return argumentError("1", args.size());
    }
}

std::string handleRpop(KVStore& kvstore, const std::vector<std::string>& args)
{
    if (args.size() != 1)
    {
        std::cerr << "Command arguments malformed" << std::endl;
        return argumentError("1", args.size());
    }
}

std::string handleLrange(KVStore& kvstore, const std::vector<std::string>& args)
{
    if (args.size() != 3)
    {
        std::cerr << "Command arguments malformed" << std::endl;
        return argumentError("3", args.size());
    }
}

std::string handleLlen(KVStore& kvstore, const std::vector<std::string>& args)
{
    if (args.size() != 1)
    {
        std::cerr << "Command arguments malformed" << std::endl;
        return argumentError("1", args.size());
    }
}

std::string handleLindex(KVStore& kvstore, const std::vector<std::string>& args)
{
    if (args.size() != 2)
    {
        std::cerr << "Command arguments malformed" << std::endl;
        return argumentError("2", args.size());
    }
}

std::string handleLset(KVStore& kvstore, const std::vector<std::string>& args)
{
    if (args.size() != 3)
    {
        std::cerr << "Command arguments malformed" << std::endl;
        return argumentError("3", args.size());
    }
}

std::string handleLrem(KVStore& kvstore, const std::vector<std::string>& args)
{
    if (args.size() != 3)
    {
        std::cerr << "Command arguments malformed" << std::endl;
        return argumentError("3", args.size());
    }
}

//TODO pub/sub
//TODO advanced data structures and commands (lists,sets,hashs,sortedset)
//TODO multi exec
