#pragma once

#include <vector>
#include <variant>
#include <memory>


struct RESPValue;

using RESPValueWrapper = std::variant<
    std::string,
    int64_t,
    std::vector<std::unique_ptr<RESPValue>>
>;

struct RESPValue {
    RESPValueWrapper value;
};