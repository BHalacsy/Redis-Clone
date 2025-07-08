#pragma once

#include <list>
#include <unordered_map>
#include <iostream>
#include <mutex>
#include <string>
#include <memory>

#include "respvalue.hpp"

class LRU {
public:
    LRU() = default;
    ~LRU() = default; //TODO maybe handle persistence here? Clear?

    void touch(const std::string& k) //Refresh in order queue
    {
        std::lock_guard lock(mtx);
        if (const auto found = keyToOrder.find(k); found != keyToOrder.end())
        {
            order.splice(order.begin(), order, found->second);
        }
        else
        {
            order.push_front(k);
            keyToOrder[k] = order.begin();
        }
    }

    std::string evict() //Remove oldest key in queue
    {
        std::lock_guard lock(mtx);
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
        std::lock_guard lock(mtx);
        if (const auto found = keyToOrder.find(k); found != keyToOrder.end()) {
            order.erase(found->second);
            keyToOrder.erase(found);
        }
    }

    void clear()
    {
        std::lock_guard lock(mtx);
        order.clear();
        keyToOrder.clear();
    }

private:
    std::list<std::string> order; //order queue
    std::unordered_map<std::string, std::list<std::string>::iterator> keyToOrder; //key->iterator in order queue
    std::mutex mtx;
};