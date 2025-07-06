#include <sstream>
#include <format>

#include "pubsub.hpp"


void PubSub::subscribe(const std::string& channel, int sock) {
    std::lock_guard<std::mutex> lock(mtx);
    channels[channel].push_back(sock);
}

int PubSub::publish(const std::string& channel, const std::string& message) {
    std::lock_guard<std::mutex> lock(mtx);

    auto found = channels.find(channel);
    if (found == channels.end()) return 0;

    const std::string formatted = formatMessage(channel, message);
    int count = 0;

    for (int sock : found->second) {
        ssize_t sent = send(sock, formatted.c_str(), formatted.size(), 0);
        if (sent >= 0) ++count;
        // TODO error handle fail maybe
    }
    return count;
}

std::string PubSub::formatMessage(const std::string& channel, const std::string& message) {
    return std::format("*3\r\n$7\r\nmessage\r\n${}\r\n{}\r\n${}\r\n{}\r\n", channel.size(), channel, message.size(), message);
}
