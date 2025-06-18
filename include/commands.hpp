#pragma once

#include <string>

//commands for switching:
enum class Commands
{
    SET,
    GET,
    DEL,
    EXISTS,
    UNKNOWN
};

Commands strToCmd(const std::string& cmd)
{
    if (cmd == "SET") return Commands::SET;
    if (cmd == "GET") return Commands::GET;
    if (cmd == "DEL") return Commands::DEL;
    if (cmd == "EXISTS") return Commands::EXISTS;
    return Commands::UNKNOWN;
}


