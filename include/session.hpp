#pragma once
#include <string>
#include <unordered_set>

struct Session {
    int clientSock;
    std::string clientAddress;

    std::string partialCommand;

    //Authentication here if needed
};