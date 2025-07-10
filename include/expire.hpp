#pragma once

#include <string>
#include <unordered_map>
#include <chrono>
#include <mutex>
#include <tbb/concurrent_hash_map.h>


class Expiration{
public:
    void setExpiry(const std::string& key, int seconds);
    int getTTL(const std::string& key);
    void removeAllExp(); //Clear all expired keys
    std::optional<std::string> removeKeyExp(const std::string& key); //Specific key check (faster than checking all)
    void erase(const std::string& key);
    void clear();

private:
    tbb::concurrent_hash_map<std::string, std::chrono::steady_clock::time_point> expTable; //Key->Time of expiration
};
