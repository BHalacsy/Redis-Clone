#pragma once

#include <deque>
#include <variant>
#include <unordered_set>
#include <unordered_map>
#include <string>

//for when implementing all various datatypes, maybe have vector<RESPvalue>
enum class storeType {STR, LIST, SET, HASH};

struct RESPValue
{
    storeType type;
    std::variant<
    std::string, //ints are stored as strings
    std::deque<std::string>,
    std::unordered_set<std::string>,
    std::unordered_map<std::string, std::string>
    > value;
};