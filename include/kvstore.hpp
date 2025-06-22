#pragma once
#include <unordered_map>
#include <chrono>
#include <string>
#include <RESPtype.hpp>
#include <optional>

class KVStore {
public:
    KVStore();
    ~KVStore();

    void removeExp(const std::string& k);

    std::optional<std::string> get(const std::string& k);
    bool set(const std::string& k, const std::string& v);
    int del(const std::vector<std::string>& args);
    int exists(const std::vector<std::string>& args);
    std::optional<int> incr(const std::string& k);
    std::optional<int> dcr(const std::string& k);
    bool expire(const std::string& k, int s);
    int ttl(const std::string& k);
    void flushall();
    std::vector<std::optional<std::string>> mget(const std::vector<std::string>& args);

private:
    std::unordered_map<std::string,std::string> dict;
    std::unordered_map<std::string,std::chrono::steady_clock::time_point> expTable;
    std::mutex mtx;
};
