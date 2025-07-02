#include <kvstore.hpp>
#include <RESPtype.hpp>

#include <iostream>
#include <mutex>

KVStore::KVStore(const bool persist, const std::string& fileName) : persistenceToggle(persist), persistenceFile(fileName)
{
    if (persistenceToggle) loadFromDisk();
}

KVStore::~KVStore()
{
    if (persistenceToggle) saveToDisk();
}

//Helpers
void KVStore::removeExp(const std::string& k)
{
    auto found = expTable.find(k);

    if (found != expTable.end() && std::chrono::steady_clock::now() >= found->second)
    {
        dict.erase(k);
        expTable.erase(found);
    }
}
void loadFromDisk()
{

}
void saveToDisk()
{

}

//Basics
std::optional<std::string> KVStore::get(const std::string& k)
{
    std::lock_guard lock(mtx);
    try
    {
        removeExp(k);
        auto found = dict.find(k);
        if (found != dict.end()) return std::get<std::string>(found->second.value);
        return std::nullopt;
    } catch (std::exception& e) {
        std::cerr << "Fail in get: " << e.what() << std::endl;
        return std::nullopt;
    }
}
bool KVStore::set(const std::string& k, const std::string& v)
{
    std::lock_guard lock(mtx);
    auto val = RESPValue{valueType::STR, v};
    try
    {
        if (dict.contains(k)) dict[k] = val;
        else dict.insert({k,val});
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
        dict[k] = RESPValue{valueType::STR, "1"};
        return 1;
    }

    try { ret = std::stoi(std::get<std::string>(found->second.value)); }
    catch (std::exception& e) {
        std::cerr << "Fail in incr: " << e.what() << std::endl;
        return std::nullopt;
    }

    ret++;
    found->second.value = std::string(std::to_string(ret));
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
        dict[k] = RESPValue{valueType::STR, "-1"};
        return -1;
    }

    try{ ret = std::stoi(std::get<std::string>(found->second.value));}
    catch (std::exception& e) {
        std::cerr << "Fail in dcr: " << e.what() << std::endl;
        return std::nullopt;
    }

    ret--;
    found->second.value = std::string(std::to_string(ret));
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
std::vector<std::optional<std::string>> KVStore::mget(const std::vector<std::string>& args)
{
    std::vector<std::optional<std::string>> result;
    for (const auto& i : args) {
        result.push_back(get(i));
    }
    return result;
}


// int lpush(const std::vector<std::string>& args)
// {
//
// }
// int rpush(const std::vector<std::string>& args)
// {
//
// }
// std::optional<std::string> lpop(const std::string& k)
// {
//
// }
// std::optional<std::string> rpop(const std::string& k)
// {
//
// }
// std::vector<std::optional<std::string>> lrange(const std::string& k, const int& start, const int& stop)
// {
//
// }
// int llen(const std::string& k)
// {
//
// }
// std::optional<std::string> lindex(const std::string& k, const int& index)
// {
//
// }
// bool lset(const std::string& k, const int& index, const std::string& v)
// {
//
// }
// int lrem(const std::string& k, const int& count, const std::string& v)
// {
//
// }
//
// int sadd(const std::vector<std::string>& args)
// {
//
// }
// int srem(const std::vector<std::string>& args)
// {
//
// }
// bool sismember(const std::string& k, const std::string& v)
// {
//
// }
// std::vector<std::optional<std::string>> smembers(const std::string& k)
// {
//
// }
// int scard(const std::string& k)
// {
//
// }
// std::vector<std::optional<std::string>> spop(const std::string& k, const int& count)
// {
//
// }
//
// int hset(const std::string& k, const std::string& f, const std::string& v)
// {
//
// }
// std::optional<std::string> hget(const std::string& k, const std::string& f)
// {
//
// }
// int hdel(const std::vector<std::string>& args)
// {
//
// }
// int hexists(const std::vector<std::string>& args)
// {
//
// }
// int hlen(const std::string& k)
// {
//
// }
// std::vector<std::optional<std::string>> hkeys(const std::string& k)
// {
//
// }
// std::vector<std::optional<std::string>> hvals(const std::string& k)
// {
//
// }
// bool hmset(const std::vector<std::string>& args)
// {
//
// }
// std::vector<std::optional<std::string>> hmget(const std::vector<std::string>& args)
// {
//
// }

