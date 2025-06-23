#pragma once

#include <deque>
#include <variant>
#include <unordered_set>
#include <set>
#include <unordered_map>
#include <string>

//for when implementing all various datatypes, maybe have vector<RESPvalue>
enum class valueType {STR, LIST, SET, HASH, SSET};

struct RESPValue
{
    valueType type;
    std::variant<
    std::string, //ints are stored as strings
    std::deque<std::string>,
    std::unordered_set<std::string>,
    std::unordered_map<std::string, std::string>,
    std::set<std::string>
    > value;
};