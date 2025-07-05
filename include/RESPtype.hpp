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
#include <boost/variant2/variant.hpp>
#include <boost/serialization/deque.hpp>
#include <boost/serialization/string.hpp>

//for when implementing all various datatypes, maybe have vector<RESPvalue>
enum class storeType {STR, LIST, SET, HASH};

struct RESPValue
{
    storeType type;
    boost::variant<std::string, std::deque<std::string>,std::unordered_set<std::string>,std::unordered_map<std::string, std::string>> value;

private:
    //RESP value serialization
    friend class boost::serialization::access;
    template<class Archive> void serialize(Archive& ar, const unsigned int version)
    {
        ar & type;
        ar & value;
    }
};

// Enum serialization
template<class Archive> void serialize(Archive& ar, storeType& g, const unsigned int version)
{
    ar & reinterpret_cast<int&>(g);
}