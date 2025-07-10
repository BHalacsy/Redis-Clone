#include "expire.hpp"

#include <string>
#include <unordered_map>
#include <chrono>
#include <tbb/concurrent_hash_map.h>
#include <mutex>


//TODO expiration is not persistent, meaning keys with ttl lose the ttl and are just saved

void Expiration::setExpiry(const std::string& key, int seconds)
{
    tbb::concurrent_hash_map<std::string, std::chrono::steady_clock::time_point>::accessor accessor;

    auto val  = std::chrono::steady_clock::now() + std::chrono::seconds(seconds);
    if (!expTable.find(accessor, key)) expTable.insert(accessor, key);

    accessor->second = val;
}

int Expiration::getTTL(const std::string& key)
{
    tbb::concurrent_hash_map<std::string, std::chrono::steady_clock::time_point>::accessor accessor;

    if (!expTable.find(accessor, key)) return -1;

    const auto dur = accessor->second - std::chrono::steady_clock::now();
    return static_cast<int>(std::chrono::duration_cast<std::chrono::seconds>(dur).count());
}

void Expiration::removeAllExp()
{
    for (auto i = expTable.begin(); i != expTable.end();)
    {
        tbb::concurrent_hash_map<std::string, std::chrono::steady_clock::time_point>::accessor accessor;
        const std::string& key = i->first;

        if (expTable.find(accessor, key) && std::chrono::steady_clock::now() >= accessor->second)
        {
            expTable.erase(accessor);
        }
        ++i;
    }
}

std::optional<std::string> Expiration::removeKeyExp(const std::string& key)
{
    tbb::concurrent_hash_map<std::string, std::chrono::steady_clock::time_point>::accessor accessor;
    if (expTable.find(accessor, key) && std::chrono::steady_clock::now() >= accessor->second)
    {
        auto keyCopy = accessor->first;
        expTable.erase(accessor);
        return keyCopy;
    }
    return std::nullopt;
}

void Expiration::erase(const std::string& key)
{
    expTable.erase(key);
}

void Expiration::clear()
{
    expTable.clear();
}




