#include "expire.hpp"

#include <string>
#include <unordered_map>
#include <chrono>
#include <cmath>
#include <mutex>

void Expiration::setExpiry(const std::string& key, int seconds)
{
    std::lock_guard lock(mtx);
    expTable[key] = std::chrono::steady_clock::now() + std::chrono::seconds(seconds);
}

int Expiration::getTTL(const std::string& key)
{
    std::lock_guard lock(mtx);
    const auto found = expTable.find(key);
    if (found == expTable.end()) return -1;
    const auto dur = found->second - std::chrono::steady_clock::now();
    return static_cast<int>(std::chrono::duration_cast<std::chrono::seconds>(dur).count());
}

void Expiration::removeAllExp(std::unordered_map<std::string, RESPValue>& dict)
{
    std::lock_guard lock(mtx);
    for (auto i = expTable.begin(); i != expTable.end();)
    {
        if (std::chrono::steady_clock::now() >= i->second)
        {
            dict.erase(i->first);
            i = expTable.erase(i);
        }
        else ++i;
    }
}

void Expiration::removeKeyExp(const std::string& key, std::unordered_map<std::string, RESPValue>& dict)
{
    std::lock_guard lock(mtx);
    if (const auto found = expTable.find(key); found != expTable.end() && std::chrono::steady_clock::now() >= found->second)
    {
        dict.erase(key);
        expTable.erase(found);
    }
}

void Expiration::clear()
{
    std::lock_guard lock(mtx);
    expTable.clear();
}

void Expiration::erase(const std::string& key)
{
    std::lock_guard lock(mtx);
    expTable.erase(key);
}





