#pragma once

#include <deque>
#include <variant>
#include <unordered_set>
#include <unordered_map>
#include <string>
#include <boost/serialization/variant.hpp>
#include <boost/variant/variant.hpp>
#include <boost/serialization/unordered_map.hpp>
#include <boost/serialization/unordered_set.hpp>
#include <boost/serialization/deque.hpp>
#include <boost/serialization/string.hpp>

enum class storeType {STR, LIST, SET, HASH};

template<class Archive> void serialize(Archive& ar, storeType& t, const unsigned version) //storeType Enum class serialization
{
    ar & reinterpret_cast<int&>(t);
}

struct RESPValue
{
    storeType type; //For type checking or switch casing
    boost::variant<
        std::string,
        std::deque<std::string>,
        std::unordered_set<std::string>,
        std::unordered_map<std::string, std::string>> value; //Uses boost for serialization

private:
    friend class boost::serialization::access;
    template<class Archive> void serialize(Archive& ar, const unsigned version) //RESPvalue struct serialization
    {
        ar & type; //Implemented above
        ar & value;
    }
};

