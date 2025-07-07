#pragma once
#include <string>
#include <unordered_set>

struct Session {
    //Basic client info
    int clientSock;
    std::string clientAddress;

    //For handling partially received cmds
    std::string partialCommand;

    //Transaction necessities



    //Authentication here if needed
};