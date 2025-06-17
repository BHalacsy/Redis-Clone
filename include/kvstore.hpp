#pragma once
#include <unordered_map>
#include <string>
#include <RESPtype.hpp>

class KVStore {
public:
    KVStore();
    ~KVStore();

    std::string get(std::string k);
    void set(std::string k, std::string v);

private:
    std::unordered_map<std::string,std::string> dict;
};
