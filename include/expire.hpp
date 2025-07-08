#pragma once

#include <string>
#include <unordered_map>
#include <tbb/concurrent_hash_map.h>
#include <chrono>
#include <mutex>

#include "respvalue.hpp"
#include "LRU.hpp"

class Expiration{
public:
    void setExpiry(const std::string& key, int seconds);
    int getTTL(const std::string& key);
    void removeAllExp(tbb::concurrent_hash_map<std::string, RESPValue>& dict, LRU& lru); //Clear all expired keys
    std::optional<std::string> removeKeyExp(const std::string& key); //Specific key check (faster than checking all)
    void erase(const std::string& key);
    void clear();

private:
    std::unordered_map<std::string, std::chrono::steady_clock::time_point> expTable; //Key->Time of expiration
    std::mutex mtx;
};
