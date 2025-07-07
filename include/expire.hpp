#pragma once

#include <string>
#include <unordered_map>
#include <tbb/concurrent_hash_map.h>
#include <chrono>
#include <mutex>

#include "RESPtype.hpp"

class Expiration{
public: //TODO clean unused methods and order them
    void setExpiry(const std::string& key, int seconds);
    int getTTL(const std::string& key);
    void removeAllExp(tbb::concurrent_hash_map<std::string, RESPValue>& dict);
    void removeKeyExp(const std::string& key, tbb::concurrent_hash_map<std::string, RESPValue>& dict);
    void clear();
    void erase(const std::string& key);

private:
    std::unordered_map<std::string, std::chrono::steady_clock::time_point> expTable; //Key->Time of expiration
    std::mutex mtx; //Thread lock
};
