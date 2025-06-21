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
    auto found = expTable.find(k);

    if (found != expTable.end() && std::chrono::steady_clock::now() >= found->second)
    {
        dict.erase(k);
        expTable.erase(found);
    }
}

std::optional<std::string> KVStore::get(const std::string& k)
{
    std::lock_guard lock(mtx);
    try
    {
        removeExp(k);
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
        expTable.erase(k);
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
            removeExp(k);
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
    removeExp(k);
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
    removeExp(k);
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

bool KVStore::expire(const std::string& k, int s)
{
    std::lock_guard lock(mtx);
    if (!dict.contains(k)) return false;
    expTable[k] = std::chrono::steady_clock::now() + std::chrono::seconds(s);
    return true;
}

int KVStore::ttl(const std::string& k)
{
    std::lock_guard lock(mtx);
    removeExp(k);
    if (!dict.contains(k)) return -2;
    if (!expTable.contains(k)) return -1;
    auto duration = expTable.at(k) - std::chrono::steady_clock::now();
    return static_cast<int>(std::chrono::duration_cast<std::chrono::seconds>(duration).count());
}

void KVStore::flushall()
{
    std::lock_guard lock(mtx);
    expTable.clear();
    dict.clear();
    //TODO clear file from persistence
}
