#include <iostream>
#include <mutex>
#include <fstream>
#include <filesystem>
#include <boost/variant.hpp>
#include <utility>

#include "kvstore.hpp"
#include "RESPtype.hpp"

KVStore::KVStore(const bool persist, const std::string& fileName) : persistenceToggle(persist), snapshotManager(fileName) //TODO maybe change to be nicer and more specific
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
    if (const auto found = expTable.find(k); found != expTable.end() && std::chrono::steady_clock::now() >= found->second)
    {
        dict.erase(k);
        expTable.erase(found);
    }
}
void KVStore::removeExpAll()
{
    for (auto i = expTable.begin(); i != expTable.end();)
    {
        if (std::chrono::steady_clock::now() >= i->second)
        {
            dict.erase(i->first);
            i = expTable.erase(i);
        }
        else
        {
            ++i;
        }
    }
}
std::optional<storeType> KVStore::getType(const std::string& k)
{
    std::lock_guard lock(mtx);
    removeExp(k);
    const auto found = dict.find(k);
    if (found == dict.end()) return std::nullopt;
    return found->second.type;
}

//Persistence
void KVStore::loadFromDisk()
{
    std::lock_guard lock(mtx);
    try
    {
        snapshotManager.load(dict);
        removeExpAll();
    }
    catch (std::exception& e) {
        std::cerr << "Fail in loadFromDisk(): " << e.what() << std::endl;
    }
}
void KVStore::saveToDisk()
{
    std::lock_guard lock(mtx);
    removeExpAll();
    try
    {
        snapshotManager.save(dict);
    }
    catch (std::exception& e) {
        std::cerr << "Fail in saveToDisk(): " << e.what() << std::endl;
    }
}

//Basics
bool KVStore::set(const std::string& k, const std::string& v)
{
    std::lock_guard lock(mtx);

    auto val = RESPValue{storeType::STR, v};
    if (dict.contains(k)) dict[k] = val;
    else dict.insert({k,val});
    expTable.erase(k);
    return true;
}
std::optional<std::string> KVStore::get(const std::string& k)
{
    std::lock_guard lock(mtx);

    try
    {
        removeExp(k);
        const auto found = dict.find(k);
        if (found == dict.end()) return std::nullopt;
        return boost::get<std::string>(found->second.value);
    } catch (std::exception& e) {
        std::cerr << "Fail in get: " << e.what() << std::endl;
        return std::nullopt;
    }
}
int KVStore::del(const std::vector<std::string>& args)
{
    std::lock_guard lock(mtx);
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
}
int KVStore::exists(const std::vector<std::string>& args)
{
    std::lock_guard<std::mutex> lock(mtx);

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
}
std::optional<int> KVStore::incr(const std::string& k)
{
    std::lock_guard lock(mtx);

    removeExp(k);
    const auto found = dict.find(k);
    int ret = 0;
    if (found == dict.end())
    {
        dict[k] = RESPValue{storeType::STR, "1"};
        return 1;
    }
    const auto val = boost::get<std::string>(found->second.value);

    try{ ret = std::stoi(val); }
    catch (std::exception& e) {
        std::cerr << "Fail in incr(" << val << "): " << e.what() << std::endl;
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
    const auto found = dict.find(k);
    int ret = 0;
    if (found == dict.end())
    {
        dict[k] = RESPValue{storeType::STR, "-1"};
        return -1;
    }
    const auto val = boost::get<std::string>(found->second.value);

    try{ ret = std::stoi(val); }
    catch (std::exception& e) {
        std::cerr << "Fail in dcr(" << val << "): " << e.what() << std::endl;
        return std::nullopt;
    }

    ret--;
    found->second.value = std::string(std::to_string(ret));
    return ret;
}
bool KVStore::expire(const std::string& k, const int s)
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
    const auto duration = expTable.at(k) - std::chrono::steady_clock::now();
    return static_cast<int>(std::chrono::duration_cast<std::chrono::seconds>(duration).count());
}
void KVStore::flushall()
{
    std::lock_guard lock(mtx);

    expTable.clear();
    dict.clear();
    snapshotManager.clear();
}
std::vector<std::optional<std::string>> KVStore::mget(const std::vector<std::string>& args)
{
    std::vector<std::optional<std::string>> ret;
    for (const auto& i : args)
    { //TODO change so not use get? locking?
        ret.push_back(get(i));
    }
    return ret;
}

//Lists
int KVStore::lpush(const std::vector<std::string>& args)
{
    std::lock_guard lock(mtx);
    const std::string& key = args[0];

    const auto found = dict.find(key);
    if (found == dict.end())
    {
        dict[key] = RESPValue{
            storeType::LIST,
            std::deque<std::string>(args.rbegin(), args.rend() - 1)
            };
        return static_cast<int>(args.size() - 1);
    }
    auto& val = boost::get<std::deque<std::string>>(found->second.value);

    for (auto i = args.rbegin(); i != args.rend() - 1; ++i)
    {
        val.push_front(*i);
    }
    return static_cast<int>(val.size());
}
int KVStore::rpush(const std::vector<std::string>& args)
{
    std::lock_guard lock(mtx);

    const std::string& key = args[0];
    const auto found = dict.find(key);
    if (found == dict.end())
    {
        dict[key] = RESPValue{
            storeType::LIST,
            std::deque<std::string>(args.begin() + 1, args.end())
            };
        return static_cast<int>(args.size() - 1);
    }
    auto& val = boost::get<std::deque<std::string>>(found->second.value);

    for (auto i = args.begin() + 1; i != args.end(); ++i)
    {
        val.push_back(*i);
    }
    return static_cast<int>(val.size());
}
std::optional<std::string> KVStore::lpop(const std::string& k)
{
    std::lock_guard lock(mtx);

    const auto found = dict.find(k);
    if (found == dict.end()) return std::nullopt;
    auto& val = boost::get<std::deque<std::string>>(found->second.value);

    if (val.empty()) return std::nullopt;
    std::string ret = val.front();
    val.pop_front();
    return ret;
}
std::optional<std::string> KVStore::rpop(const std::string& k)
{
    std::lock_guard lock(mtx);

    const auto found = dict.find(k);
    if (found == dict.end()) return std::nullopt;
    auto& val = boost::get<std::deque<std::string>>(found->second.value);

    if (val.empty()) return std::nullopt;
    std::string ret = val.back();
    val.pop_back();
    return ret;
}
std::vector<std::optional<std::string>> KVStore::lrange(const std::string& k, const int& start, const int& stop)
{
    std::lock_guard lock(mtx);
    std::vector<std::optional<std::string>> ret;

    const auto found = dict.find(k);
    if (found == dict.end())
    {
        ret.emplace_back(std::nullopt);
        return ret;
    }
    auto& val = boost::get<std::deque<std::string>>(found->second.value);

    int trueStart = start < 0 ? static_cast<int>(val.size()) + start : start;
    int trueStop = stop < 0 ? static_cast<int>(val.size()) + stop : stop;
    trueStart = std::max(0, trueStart);
    trueStop = std::min(static_cast<int>(val.size()) - 1, trueStop);

    if (trueStart > trueStop) return ret;

    for (int i = trueStart; i <= trueStop; ++i) {
        ret.emplace_back(val[i]);
    }
    return ret;
}
int KVStore::llen(const std::string& k)
{
    std::lock_guard lock(mtx);

    const auto found = dict.find(k);
    if (found == dict.end()) return 0;
    const auto& val = boost::get<std::deque<std::string>>(found->second.value);

    return static_cast<int>(val.size());
}
std::optional<std::string> KVStore::lindex(const std::string& k, const int& index)
{
    std::lock_guard lock(mtx);

    const auto found = dict.find(k);
    if (found == dict.end()) return std::nullopt;
    auto& val = boost::get<std::deque<std::string>>(found->second.value);
    if (static_cast<int>(val.size()) <= index) return std::nullopt;
    return val.at(index);
}
bool KVStore::lset(const std::string& k, const int& index, const std::string& v)
{
    std::lock_guard lock(mtx);

    const auto found = dict.find(k);
    if (found == dict.end()) return false;
    auto& val = boost::get<std::deque<std::string>>(found->second.value);

    if (static_cast<int>(val.size()) <= index || index < 0) return false;
    val.at(index) = v;
    return true;
}
int KVStore::lrem(const std::string& k, const int& count, const std::string& v)
{
    std::lock_guard lock(mtx);

    const auto found = dict.find(k);
    if (found == dict.end()) return 0;
    auto& val = boost::get<std::deque<std::string>>(found->second.value);
    int removed = 0;

    if (count > 0)
    {
        for (auto i = val.begin(); i != val.end() && removed < count;)
        {
            if (*i == v)
            {
                i = val.erase(i);
                removed++;
            }
            else ++i;
        }
    }
    else if (count < 0)
    {
        for (auto i = val.rbegin(); i != val.rend() && removed < -count;)
        {
            if (*i == v)
            {
                i = std::deque<std::string>::reverse_iterator(val.erase(std::next(i).base())); //gpt-ed this line whole fck
                removed++;
            }
            else ++i;
        }
    }
    else
    {
        for (auto it = val.begin(); it != val.end();)
        {
            if (*it == v)
            {
                it = val.erase(it);
                removed++;
            }
            else ++it;
        }
    }
    return removed;
}

//Sets
int KVStore::sadd(const std::vector<std::string>& args)
{
        std::lock_guard lock(mtx);
        const std::string& key = args[0];
        const auto found = dict.find(key);
        if (found == dict.end())
        {
            dict[key] = RESPValue{
                storeType::SET,
                std::unordered_set<std::string>(args.begin() + 1, args.end())
                };
            return static_cast<int>(args.size() - 1);
        }
        auto& val = boost::get<std::unordered_set<std::string>>(found->second.value);
        int added = 0;


        for (auto i = args.begin() + 1; i != args.end(); ++i)
        {
            if (val.insert(*i).second) //pair second bool if new
            {
                added++;
            }
        }
        return added;
}
int KVStore::srem(const std::vector<std::string>& args)
{
    std::lock_guard lock(mtx);
    const std::string& key = args[0];

    const auto found = dict.find(key);
    if (found == dict.end()) return 0;

    auto& val = boost::get<std::unordered_set<std::string>>(found->second.value);
    int removed = 0;

    for (auto i = args.begin() + 1; i != args.end(); ++i)
    {
        if (val.erase(*i))
        {
            removed++;
        }
    }
    return removed;
}
bool KVStore::sismember(const std::string& k, const std::string& v)
{
    std::lock_guard lock(mtx);

    const auto found = dict.find(k);
    if (found == dict.end()) return false;
    const auto& val = boost::get<std::unordered_set<std::string>>(found->second.value);

    return val.contains(v);
}
std::vector<std::optional<std::string>> KVStore::smembers(const std::string& k)
{
    std::lock_guard lock(mtx);
    std::vector<std::optional<std::string>> ret{};

    const auto found = dict.find(k);
    if (found == dict.end()) return ret;
    const auto& val = boost::get<std::unordered_set<std::string>>(found->second.value);

    for (const auto& i : val)
    {
        ret.emplace_back(i);
    }
    return ret;
}
int KVStore::scard(const std::string& k)
{
    std::lock_guard lock(mtx);

    const auto found = dict.find(k);
    if (found == dict.end()) return 0;
    const auto& val = boost::get<std::unordered_set<std::string>>(found->second.value);

    return static_cast<int>(val.size());
}
std::vector<std::optional<std::string>> KVStore::spop(const std::string& k, const int& count)
{
    std::lock_guard lock(mtx);
    std::vector<std::optional<std::string>> ret{};

    const auto found = dict.find(k);
    if (found == dict.end() || count == 0) return ret;

    auto& val = boost::get<std::unordered_set<std::string>>(found->second.value);
    for (int i = 0; i < count && !val.empty(); ++i)
    {
        //TODO make randomized pop
        auto it = val.begin();
        ret.emplace_back(*it);
        val.erase(it);
    }
    return ret;
}

//Hashes
int KVStore::hset(const std::vector<std::string>& args)
{

    std::lock_guard lock(mtx);
    const std::string& key = args[0];
    int added = 0;

    if (args.size() < 3 || args.size() % 2 == 0) return false;

    const auto found = dict.find(key);
    if (found == dict.end())
    {
        std::unordered_map<std::string, std::string> newMap;
        for (auto i = 1; i < args.size(); i += 2)
        {
            newMap[args[i]] = args[i + 1];
            added++;
        }
        dict[key] = RESPValue{storeType::HASH, newMap};
        return added;
    }
    auto& val = boost::get<std::unordered_map<std::string, std::string>>(found->second.value);

    for (auto i = 1; i < args.size(); i += 2)
    {
        if (!val.contains(args[i])) added++;
        val[args[i]] = args[i + 1];
    }
    return added;
}
std::optional<std::string> KVStore::hget(const std::string& k, const std::string& f)
{
    std::lock_guard lock(mtx);

    const auto found = dict.find(k);
    if (found == dict.end()) return std::nullopt;
    auto& val = boost::get<std::unordered_map<std::string,std::string>>(found->second.value);

    const auto foundField = val.find(f);
    if (foundField == val.end()) return std::nullopt;

    return foundField->second;
}
int KVStore::hdel(const std::vector<std::string>& args)
{
    std::lock_guard lock(mtx);
    const std::string& key = args[0];

    const auto found = dict.find(key);
    if (found == dict.end()) return 0;
    auto& val = boost::get<std::unordered_map<std::string,std::string>>(found->second.value);
    int removed = 0;

    for (auto it = args.begin() + 1; it != args.end(); ++it)
    {
        if (val.erase(*it) > 0)
        {
            removed++;
        }
    }
    return removed;
}
bool KVStore::hexists(const std::string& k, const std::string& f)
{
    std::lock_guard lock(mtx);

    const auto found = dict.find(k);
    if (found == dict.end()) return false;
    auto& val = boost::get<std::unordered_map<std::string,std::string>>(found->second.value);

    if (const auto foundField = val.find(f); foundField == val.end()) return false;

    return true;
}
int KVStore::hlen(const std::string& k)
{
    std::lock_guard lock(mtx);

    const auto found = dict.find(k);
    if (found == dict.end()) return 0;
    const auto& val = boost::get<std::unordered_map<std::string,std::string>>(found->second.value);

    return static_cast<int>(val.size());
}
std::vector<std::optional<std::string>> KVStore::hkeys(const std::string& k)
{
    std::lock_guard lock(mtx);
    std::vector<std::optional<std::string>> ret{};

    const auto found = dict.find(k);
    if (found == dict.end()) return ret;
    auto& val = boost::get<std::unordered_map<std::string,std::string>>(found->second.value);

    for (const auto& i : val)
    {
        ret.emplace_back(std::make_optional(i.first));
    }
    //would use std::sort(ret.begin(), ret.end()) but not needed as redis doesn't do it either (same for hvals)
    return ret;
}
std::vector<std::optional<std::string>> KVStore::hvals(const std::string& k)
{
    std::lock_guard lock(mtx);
    std::vector<std::optional<std::string>> ret{};

    const auto found = dict.find(k);
    if (found == dict.end()) return ret;
    const auto& val = boost::get<std::unordered_map<std::string,std::string>>(found->second.value);

    for (const auto& i : val)
    {
        ret.emplace_back(std::make_optional(i.second));
    }
    return ret;
}
std::vector<std::optional<std::string>> KVStore::hmget(const std::vector<std::string>& args)
{
    std::lock_guard lock(mtx);
    const std::string& key = args[0];
    std::vector<std::optional<std::string>> ret{};

    const auto found = dict.find(key);
    if (found == dict.end())
    {
        ret.resize(args.size() - 1, std::nullopt); //a bit messy but will do?
        return ret;
    }
    auto& val = boost::get<std::unordered_map<std::string, std::string>>(found->second.value);

    for (auto i = 1; i < args.size(); ++i)
    {
        const auto field = val.find(args[i]);

        if (field == val.end()) ret.emplace_back(std::nullopt);
        else ret.emplace_back(std::make_optional(field->second));
    }

    return ret;
}

