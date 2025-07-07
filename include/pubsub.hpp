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
    //TODO unsub/unsuball

private:
    std::unordered_map<std::string, std::vector<int>> channels;
    std::unordered_map<int, std::vector<std::string>> users;
    std::mutex mtx;

    static std::string formatMessage(const std::string& channel, const std::string& message);
};
