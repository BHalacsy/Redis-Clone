#pragma once

#include <string>
#include <unordered_map>
#include <vector>
#include <mutex>
#include <sys/socket.h>

class PubSub {
public:
    int subscribe(const std::string& channel, int sock);
    int publish(const std::string& channel, const std::string& message);
    int unsubscribe(const std::string& channel, int sock);
    void unsubscribeAll(int sock);

private:
    std::unordered_map<std::string, std::vector<int>> channels; //Holds all socks subbed
    std::unordered_map<int, std::vector<std::string>> users; //Holds all channels subbed to (For tracking sub count per user)
    std::mutex mtx;

    static std::string formatMessage(const std::string& channel, const std::string& message); //Used for pubing
};
