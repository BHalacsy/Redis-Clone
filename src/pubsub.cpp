#include <sstream>
#include <format>

#include "pubsub.hpp"


int PubSub::subscribe(const std::string& channel, const int sock)
{
    std::lock_guard lock(mtx);
    channels[channel].push_back(sock);
    users[sock].push_back(channel);
    return static_cast<int>(users[sock].size());
}

int PubSub::publish(const std::string& channel, const std::string& message)
{
    std::lock_guard lock(mtx);

    if (!channels.contains(channel)) return 0;

    const std::string pubbedMessage = formatMessage(channel, message);
    int count = 0;

    for (const int subs : channels[channel])
    {
        ssize_t sent = send(subs, pubbedMessage.c_str(), pubbedMessage.size(), 0);
        if (sent >= 0) ++count;
        // TODO error handle fail maybe
    }
    return count;
}

int PubSub::unsubscribe(const std::string& channel, int sock)
{
    std::lock_guard lock(mtx);
    if (channels.contains(channel))
    {
        std::erase(channels[channel], sock);
        if (channels[channel].empty()) {
            channels.erase(channel);
        }
    }
    std::erase(users[sock], channel);
    return static_cast<int>(users[sock].size());
}

void PubSub::unsubscribeAll(const int sock)
{
    std::lock_guard lock(mtx);
    if (!users.contains(sock)) return;

    for (const auto& channel : users[sock]) {
        if (channels.contains(channel)) {
            std::erase(channels[channel], sock);
            if (channels[channel].empty()) {
                channels.erase(channel);
            }
        }
    }
    users.erase(sock);
}


std::string PubSub::formatMessage(const std::string& channel, const std::string& message) {
    return std::format("*3\r\n$7\r\nmessage\r\n${}\r\n{}\r\n${}\r\n{}\r\n", channel.size(), channel, message.size(), message);
}
