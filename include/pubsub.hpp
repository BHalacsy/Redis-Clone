#pragma once

#include <string>
#include <unordered_map>
#include <vector>
#include <mutex>
#include <sys/socket.h>

class PubSub {
public:
    void subscribe(const std::string& channel, int sock);
    int publish(const std::string& channel, const std::string& message);
    //TODO unsub/unsuball

private:
    std::unordered_map<std::string, std::vector<int>> channels;
    std::mutex mtx;

    static std::string formatMessage(const std::string& channel, const std::string& message);
};
