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
    bool transActive = false;
    std::vector<std::vector<std::string>> transQueue;

    //TODO Authentication here in future
};
