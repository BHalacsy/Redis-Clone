#pragma once

#include <vector>
#include <variant>
#include <memory>
#include <string>

//for when implementing all various datatypes, maybe have vector<RESPvalue>
struct RESPValue;

using RESPValueWrapper = std::variant<
    std::string,
    int64_t,
    std::vector<std::string>
>;

struct RESPValue {
    RESPValueWrapper value;
};