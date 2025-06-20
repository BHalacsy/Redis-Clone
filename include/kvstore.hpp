#pragma once
#include <unordered_map>
#include <string>
#include <RESPtype.hpp>
#include <optional>

class KVStore {
public:
    KVStore();
    ~KVStore();

    std::optional<std::string> get(const std::string& k);
    bool set(const std::string& k, const std::string& v);
    int del(const std::vector<std::string>& args);
    int exists(const std::vector<std::string>& args) const;

private:
    std::unordered_map<std::string,std::string> dict;
    std::mutex lock;
};
