#pragma once

#include <vector>
#include <variant>
#include <memory>
#include <string>


struct RESPValue;

using RESPValueWrapper = std::variant<
    std::string,
    int64_t,
    std::vector<std::string>
>;

struct RESPValue {
    RESPValueWrapper value;
};