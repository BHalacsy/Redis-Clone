#include <kvstore.hpp>
#include <RESPtype.hpp>

#include <iostream>
#include <mutex>

KVStore::KVStore()
{
    //Maybe add persistence to rebuild in mem db, using fstream on a file.

}

KVStore::~KVStore()
{
    //Save everything to the disk?
}

void KVStore::removeExp(const std::string& k)
{
    expTable;
    return;
}

std::optional<std::string> KVStore::get(const std::string& k)
{
    std::lock_guard lock(mtx);
    try
    {
        auto found = dict.find(k);
        if (found != dict.end()) return found->second;
        return std::nullopt;
    } catch (std::exception& e) {
        std::cerr << "Fail in get: " << e.what() << std::endl;
        return std::nullopt;
    }
}

bool KVStore::set(const std::string& k, const std::string& v)
{
    std::lock_guard lock(mtx);
    try
    {
        if (dict.contains(k)) dict[k] = v;
        else dict.insert({k,v});
        return true;
    } catch (std::exception& e) {
        std::cerr << "Fail in set: " << e.what() << std::endl;
        return false;
    }
}

int KVStore::del(const std::vector<std::string>& args)
{
    std::lock_guard lock(mtx);
    try
    {
        int deleted = 0;
        for (const auto& k : args)
        {

            if (dict.contains(k))
            {
                dict.erase(k);
                deleted++;
            }
        }
        return deleted;
    } catch (std::exception& e) {
        std::cerr << "Fail in del: " << e.what() << std::endl;
        return 0;
    }
}

int KVStore::exists(const std::vector<std::string>& args)
{
    std::lock_guard<std::mutex> lock(mtx);
    try
    {
        int exist = 0;
        for (const auto& k : args)
        {
            if (dict.contains(k))
            {
                exist++;
            }
        }
        return exist;

    } catch (std::exception& e) {
        std::cerr << "Fail in exists: " << e.what() << std::endl;
        return 0;
    }
}

std::optional<int> KVStore::incr(const std::string& k)
{
    std::lock_guard lock(mtx);
    auto found = dict.find(k);
    int ret = 0;
    if (found == dict.end())
    {
        dict[k] = "1";
        return 1;
    }

    try { ret = std::stoi(found->second); }
    catch (std::exception& e) {
        std::cerr << "Fail in incr: " << e.what() << std::endl;
        return std::nullopt;
    }

    ret++;
    found->second = std::to_string(ret);
    return ret;
}

std::optional<int> KVStore::dcr(const std::string& k)
{
    std::lock_guard lock(mtx);
    auto found = dict.find(k);
    int ret = 0;
    if (found == dict.end())
    {
        dict[k] = "-1";
        return -1;
    }

    try{ ret = std::stoi(found->second);}
    catch (std::exception& e) {
        std::cerr << "Fail in dcr: " << e.what() << std::endl;
        return std::nullopt;
    }

    ret--;
    found->second = std::to_string(ret);
    return ret;
}

