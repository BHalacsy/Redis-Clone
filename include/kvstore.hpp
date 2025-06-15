#pragma once
#include <unordered_map>
#include <string>
#include <RESPtype.hpp>

class KVStore {
public:
    KVStore();
    ~KVStore();

    RESPValue get(std::string k);
    void set(std::string k, RESPValue v);

private:
    std::unordered_map<std::string,RESPValue> dict;
};
