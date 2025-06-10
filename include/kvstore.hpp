#pragma once
#include <unordered_map>
#include <string>
#include <variant>

using values = std::variant<int, std::string>;

class KVStore {
public:
    KVStore();
    ~KVStore();

    values get(std::string k);
    void set(std::string k, values v);

private:
    std::unordered_map<std::string,values> dict;
};
