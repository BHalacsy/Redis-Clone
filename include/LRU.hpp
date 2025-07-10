#pragma once

#include <list>
#include <unordered_map>
#include <iostream>
#include <mutex>
#include <string>
#include <tbb/concurrent_hash_map.h>

class LRU {
public:
    LRU() = default;
    ~LRU() = default;

    void touch(const std::string& k) //Refresh in order queue
    {
        tbb::concurrent_hash_map<std::string, std::list<std::string>::iterator>::accessor accessor;
        if (!keyToOrder.find(accessor, k))
        {
            order.push_front(k);
            keyToOrder.insert(accessor,k);
            accessor->second = order.begin();
        }
        else
        {
            order.splice(order.begin(), order, accessor->second);
        }
    }

    std::string evict() //Removes oldest key in order queue
    {
        if (order.empty()) //not sure if needed
        {
            std::cerr << "trying to evict when empty" << std::endl;
            return "";
        }
        std::string ret = order.back();
        order.pop_back();

        keyToOrder.erase(ret);
        return ret;
    }

    void erase(const std::string& k)
    {
        tbb::concurrent_hash_map<std::string, std::list<std::string>::iterator>::accessor accessor;
        if (keyToOrder.find(accessor, k))
        {
            order.erase(accessor->second);
            keyToOrder.erase(accessor);
        }
    }

    void clear()
    {
        order.clear();
        keyToOrder.clear();
    }

private:
    std::list<std::string> order; //Order queue
    tbb::concurrent_hash_map<std::string, std::list<std::string>::iterator> keyToOrder; //Key->iterator in order queue
};