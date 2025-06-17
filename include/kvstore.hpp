#pragma once
#include <unordered_map>
#include <string>
#include <RESPtype.hpp>

class KVStore {
public:
    KVStore();
    ~KVStore();

    std::string get(const std::string& k);
    void set(const std::string& k, const std::string& v);
    void del(const std::string& k);
    bool exists(const std::string& k);
    //expire, ttl

private:
    std::unordered_map<std::string,std::string> dict;
    std::mutex lock;
};
