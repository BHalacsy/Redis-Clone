#include <vector>
#include <string>
#include <format>
#include <iostream>

#include <commands.hpp>
#include <kvstore.hpp>

Commands strToCmd(const std::string& cmd)
{
    if (cmd == "SET") return Commands::SET;
    if (cmd == "GET") return Commands::GET;
    if (cmd == "DEL") return Commands::DEL;
    if (cmd == "EXISTS") return Commands::EXISTS;
    return Commands::UNKNOWN;
}

std::string handleSet(KVStore& kvstore, const std::vector<std::string>& args)
{
    std::string response;
    if (args.size() != 2)
    {
        std::cerr << "Command arguments malformed" << std::endl;
        response = std::format("-ERR command expected {} arguments, got {} instead\r\n", 2, args.size());
    }
    else
    {
        response = kvstore.set(args[0], args[1]) ? "+OK\r\n" : "-ERR something went wrong in set\r\n";
    }
    return response;
}

std::string handleGet(KVStore& kvstore, const std::vector<std::string>& args)
{
    std::string response;
    if (args.size() != 1)
    {
        std::cerr << "Command arguments malformed" << std::endl;
        response = std::format("-ERR command expected {} arguments, got {} instead\r\n", 1, args.size());
    }
    else
    {
        auto val = kvstore.get(args[0]);
        response = val ? std::format("${}\r\n{}\r\n", val->length(), val.value()) : "$-1\r\n";
    }
    return response;
}

std::string handleDel(KVStore& kvstore, const std::vector<std::string>& args)
{
    std::string response;
    if (args.size() < 1)
    {
        std::cerr << "Command arguments malformed" << std::endl;
        response = std::format("-ERR command expected 1 or more arguments, got {} instead\r\n", args.size());
    }
    else
    {
        int deleted = kvstore.del(args);
        response = std::format(":{}\r\n",deleted);
    }
    return response;
}

std::string handleExists(KVStore& kvstore, const std::vector<std::string>& args)
{
    std::string response;
    if (args.size() < 1)
    {
        std::cerr << "Command arguments malformed" << std::endl;
        response = std::format("-ERR command expected 1 or more arguments, got {} instead\r\n", args.size());
    }
    else
    {
        int found = kvstore.exists(args);
        response = std::format(":{}\r\n", found);
    }
    return response;
}