#include <iostream>
#include <fstream>
#include <filesystem>
#include <utility>
#include <boost/variant.hpp>

#include "kvstore.hpp"
#include "respvalue.hpp"
#include "expire.hpp"

KVStore::KVStore(const bool persist, const std::string& fileName, const int maxKeys) : persistenceToggle(persist), snapshotManager(fileName), maxSize(static_cast<size_t>(maxKeys))
{
    if (persistenceToggle) loadFromDisk();
    for (auto& [k,v] : dict)
    {
        lruManager.touch(k);
        currSize.fetch_add(1);
    }
}

KVStore::~KVStore()
{
    if (persistenceToggle) saveToDisk();
}

std::optional<storeType> KVStore::getType(const std::string& k)
{
    tbb::concurrent_hash_map<std::string, RESPValue>::const_accessor accessor;
    checkExpKey(k);

    if (!dict.find(accessor, k)) return std::nullopt;
    return accessor->second.type;
}
std::optional<std::string> KVStore::checkTypeError(const std::string& k, const storeType expected)
{
    auto type = this->getType(k);
    if (type && *type != expected) return "-ERR wrong type\r\n";
    return std::nullopt;
}
void KVStore::checkExpKey(const std::string& k)
{
    if (const auto removed = expirationManager.removeKeyExp(k))
    {
        lruManager.erase(removed.value());
        dict.erase(removed.value());
    }
}
bool KVStore::spaceLeft() const
{
    size_t currKeyCount = currSize.load();
    std::cout << "Using: " << currSize.load() << " keys Max allowed: " << maxSize << std::endl; //TODO rem only for demo
    return currKeyCount < maxSize;
}
void KVStore::evictTill()
{
    while (!spaceLeft())
    {
        std::string evicted = lruManager.evict();
        currSize.fetch_sub(1);
        std::cout << "Evicted: " << evicted << std::endl; //TODO rem only for demo
        dict.erase(evicted);
    }
} //TODO change to maybe get actual value of needed deletions and delete as needed and not loop

void KVStore::loadFromDisk()
{
    try
    {
        snapshotManager.load(dict);
    }
    catch (std::exception& e) {
        std::cerr << "Fail in loadFromDisk(): " << e.what() << std::endl;
    }
}
void KVStore::saveToDisk()
{
    expirationManager.removeAllExp();
    try
    {
        snapshotManager.save(dict);
    }
    catch (std::exception& e) {
        std::cerr << "Fail in saveToDisk(): " << e.what() << std::endl;
    }
}

int KVStore::del(const std::vector<std::string>& args)
{
    int deleted = 0;
    for (const auto& k : args)
    {
        checkExpKey(k); //Only done to give true expected delete count
        expirationManager.erase(k);
        if (tbb::concurrent_hash_map<std::string, RESPValue>::accessor accessor; dict.find(accessor, k))
        {
            dict.erase(accessor);
            currSize.fetch_sub(1);
            lruManager.erase(k);
            deleted++;
        }
    }
    return deleted;
}
int KVStore::exists(const std::vector<std::string>& args)
{
    int exist = 0;
    for (const auto& k : args)
    {
        checkExpKey(k);
        if (tbb::concurrent_hash_map<std::string, RESPValue>::accessor accessor; dict.find(accessor, k))
        {
            exist++;
        }
    }
    return exist;
}
void KVStore::flushall()
{
    expirationManager.clear();
    lruManager.clear();
    dict.clear();
    if (persistenceToggle) saveToDisk();
}

bool KVStore::set(const std::string& k, const std::string& v)
{
    tbb::concurrent_hash_map<std::string, RESPValue>::accessor accessor;
    lruManager.touch(k);

    const auto val = RESPValue{storeType::STR, v};
    if (!dict.find(accessor, k))
    {
        if (!spaceLeft()) evictTill();
        dict.insert(accessor, k);
        currSize.fetch_add(1);
    }
    accessor->second = val;
    expirationManager.erase(k);
    return true;
}
std::optional<std::string> KVStore::get(const std::string& k)
{
    try
    {
        tbb::concurrent_hash_map<std::string, RESPValue>::accessor accessor;
        checkExpKey(k);
        lruManager.touch(k);

        if (!dict.find(accessor, k)) return std::nullopt;
        return boost::get<std::string>(accessor->second.value);
    }
    catch (std::exception& e) {
        std::cerr << "Fail in get: " << e.what() << std::endl;
        return std::nullopt;
    }
}
std::optional<int> KVStore::incr(const std::string& k)
{
    int ret = 0;

    tbb::concurrent_hash_map<std::string, RESPValue>::accessor accessor;
    checkExpKey(k);
    lruManager.touch(k);

    if (!dict.find(accessor, k))
    {
        if (!spaceLeft()) evictTill();
        dict.insert({k,RESPValue{storeType::STR, "1"}});
        currSize.fetch_add(1);
        return 1;
    }
    //else
    const auto val = boost::get<std::string>(accessor->second.value);

    try{ ret = std::stoi(val); }
    catch (std::exception& e) {
        std::cerr << "Fail in incr(" << val << "): " << e.what() << std::endl;
        return std::nullopt;
    }

    ret++;
    accessor->second.value = std::string(std::to_string(ret));
    return ret;
}
std::optional<int> KVStore::dcr(const std::string& k)
{
    int ret = 0;

    tbb::concurrent_hash_map<std::string, RESPValue>::accessor accessor;
    checkExpKey(k);
    lruManager.touch(k);

    if (!dict.find(accessor, k))
    {
        if (!spaceLeft()) evictTill();
        dict.insert({k,RESPValue{storeType::STR, "-1"}});
        currSize.fetch_add(1);
        return -1;
    }
    //else
    const auto val = boost::get<std::string>(accessor->second.value);

    try{ ret = std::stoi(val); }
    catch (std::exception& e) {
        std::cerr << "Fail in dcr(" << val << "): " << e.what() << std::endl;
        return std::nullopt;
    }

    ret--;
    accessor->second.value = std::string(std::to_string(ret));
    return ret;
}
std::optional<int> KVStore::incrby(const std::string& k, const int& count)
{
    int ret = 0;

    tbb::concurrent_hash_map<std::string, RESPValue>::accessor accessor;
    checkExpKey(k);
    lruManager.touch(k);

    if (!dict.find(accessor, k))
    {
        if (!spaceLeft()) evictTill();
        dict.insert({k,RESPValue{storeType::STR, std::to_string(count)}});
        currSize.fetch_add(1);
        return count;
    }
    //else
    const auto val = boost::get<std::string>(accessor->second.value);

    try{ ret = std::stoi(val); }
    catch (std::exception& e) {
        std::cerr << "Fail in incrby(" << val << "): " << e.what() << std::endl;
        return std::nullopt;
    }

    ret = ret + count;
    accessor->second.value = std::string(std::to_string(ret));
    return ret;
}
std::optional<int> KVStore::dcrby(const std::string& k, const int& count)
{
    int ret = 0;

    tbb::concurrent_hash_map<std::string, RESPValue>::accessor accessor;
    checkExpKey(k);
    lruManager.touch(k);

    if (!dict.find(accessor, k))
    {
        if (!spaceLeft()) evictTill();
        dict.insert({k,RESPValue{storeType::STR, std::to_string(-count)}});
        currSize.fetch_add(1);
        return -count;
    }
    //else
    const auto val = boost::get<std::string>(accessor->second.value);

    try{ ret = std::stoi(val); }
    catch (std::exception& e) {
        std::cerr << "Fail in dcrby(" << val << "): " << e.what() << std::endl;
        return std::nullopt;
    }

    ret = ret - count;
    accessor->second.value = std::string(std::to_string(ret));
    return ret;
}
std::vector<std::optional<std::string>> KVStore::mget(const std::vector<std::string>& args)
{
    std::vector<std::optional<std::string>> ret;
    for (const auto& i : args)
    {
        // checkExpKey(i); //Commented out because get already does it
        // lruManager.touch(i);
        if (auto err = checkTypeError(i, storeType::STR))
        {
            ret.emplace_back(std::nullopt);
        }
        else
        {
            ret.push_back(get(i));
        }
    }
    return ret;
}
int KVStore::append(const std::string& k, const std::string& v)
{
    tbb::concurrent_hash_map<std::string, RESPValue>::accessor accessor;
    checkExpKey(k);
    lruManager.touch(k);

    if (!dict.find(accessor, k))
    {
        if (!spaceLeft()) evictTill();
        dict.insert({k, {storeType::STR, v}});
        currSize.fetch_add(1);
        return static_cast<int>(v.length());
    }

    auto& val = boost::get<std::string>(accessor->second.value);
    val += v;

    return static_cast<int>(boost::get<std::string>(accessor->second.value).size());
}

bool KVStore::expire(const std::string& k, const int s)
{
    if (tbb::concurrent_hash_map<std::string, RESPValue>::accessor accessor; !dict.find(accessor, k)) return false;
    expirationManager.setExpiry(k, s);
    return true;
}
int KVStore::ttl(const std::string& k)
{
    checkExpKey(k);
    if (tbb::concurrent_hash_map<std::string, RESPValue>::accessor accessor; !dict.find(accessor, k)) return -2;
    return expirationManager.getTTL(k);
}
bool KVStore::persist(const std::string& k)
{
    checkExpKey(k);
    if (tbb::concurrent_hash_map<std::string, RESPValue>::accessor accessor; !dict.find(accessor, k)) return false;

    expirationManager.erase(k);
    return true;
}

int KVStore::lpush(const std::vector<std::string>& args)
{
    const std::string& k = args[0];

    tbb::concurrent_hash_map<std::string, RESPValue>::accessor accessor;
    checkExpKey(k);
    lruManager.touch(k);

    if (!dict.find(accessor, k))
    {
        if (!spaceLeft()) evictTill();
        dict.insert({k,RESPValue{
            storeType::LIST,
            std::deque<std::string>(args.rbegin(), args.rend() - 1)
            }});
        currSize.fetch_add(1);
        return static_cast<int>(args.size() - 1);
    }
    //else
    auto& val = boost::get<std::deque<std::string>>(accessor->second.value);

    for (auto i = args.rbegin(); i != args.rend() - 1; ++i)
    {
        val.push_front(*i);
    }
    return static_cast<int>(val.size());
}
int KVStore::rpush(const std::vector<std::string>& args)
{
    const std::string& k = args[0];

    tbb::concurrent_hash_map<std::string, RESPValue>::accessor accessor;
    checkExpKey(k);
    lruManager.touch(k);


    if (!dict.find(accessor, k))
    {
        if (!spaceLeft()) evictTill();
        dict.insert({k, RESPValue{
            storeType::LIST,
            std::deque<std::string>(args.begin() + 1, args.end())
            }});
        currSize.fetch_add(1);
        return static_cast<int>(args.size() - 1);
    }
    //else
    auto& val = boost::get<std::deque<std::string>>(accessor->second.value);

    for (auto i = args.begin() + 1; i != args.end(); ++i)
    {
        val.push_back(*i);
    }
    return static_cast<int>(val.size());
}
std::optional<std::string> KVStore::lpop(const std::string& k)
{
    tbb::concurrent_hash_map<std::string, RESPValue>::accessor accessor;
    checkExpKey(k);
    lruManager.touch(k);

    if (!dict.find(accessor, k)) return std::nullopt;
    auto& val = boost::get<std::deque<std::string>>(accessor->second.value);

    if (val.empty()) return std::nullopt;
    std::string ret = val.front();

    val.pop_front();
    if (val.empty())
    {
        expirationManager.erase(k);
        dict.erase(accessor);
        currSize.fetch_sub(1);
        lruManager.erase(k);
    }

    return ret;
}
std::optional<std::string> KVStore::rpop(const std::string& k)
{
    tbb::concurrent_hash_map<std::string, RESPValue>::accessor accessor;
    checkExpKey(k);
    lruManager.touch(k);

    if (!dict.find(accessor, k)) return std::nullopt;
    auto& val = boost::get<std::deque<std::string>>(accessor->second.value);

    if (val.empty()) return std::nullopt;
    std::string ret = val.back();

    val.pop_back();
    if (val.empty())
    {
        expirationManager.erase(k);
        dict.erase(accessor);
        currSize.fetch_sub(1);
        lruManager.erase(k);
    }

    return ret;
}
std::vector<std::optional<std::string>> KVStore::lrange(const std::string& k, const int& start, const int& stop)
{
    std::vector<std::optional<std::string>> ret;

    tbb::concurrent_hash_map<std::string, RESPValue>::accessor accessor;
    checkExpKey(k);
    lruManager.touch(k);

    if (!dict.find(accessor, k))
    {
        ret.emplace_back(std::nullopt);
        return ret;
    }
    //else
    auto& val = boost::get<std::deque<std::string>>(accessor->second.value);

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
    tbb::concurrent_hash_map<std::string, RESPValue>::accessor accessor;
    checkExpKey(k);
    lruManager.touch(k);

    if (!dict.find(accessor, k)) return 0;
    const auto& val = boost::get<std::deque<std::string>>(accessor->second.value);

    return static_cast<int>(val.size());
}
std::optional<std::string> KVStore::lindex(const std::string& k, const int& index)
{
    tbb::concurrent_hash_map<std::string, RESPValue>::accessor accessor;
    checkExpKey(k);
    lruManager.touch(k);

    if (!dict.find(accessor, k)) return std::nullopt;
    auto& val = boost::get<std::deque<std::string>>(accessor->second.value);

    if (static_cast<int>(val.size()) <= index) return std::nullopt;
    return val.at(index);
}
bool KVStore::lset(const std::string& k, const int& index, const std::string& v)
{
    tbb::concurrent_hash_map<std::string, RESPValue>::accessor accessor;
    checkExpKey(k);
    lruManager.touch(k);

    if (!dict.find(accessor, k)) return false;
    auto& val = boost::get<std::deque<std::string>>(accessor->second.value);

    if (static_cast<int>(val.size()) <= index || index < 0) return false;
    val.at(index) = v;
    return true;
}
int KVStore::lrem(const std::string& k, const int& count, const std::string& v)
{
    int removed = 0;

    tbb::concurrent_hash_map<std::string, RESPValue>::accessor accessor;
    checkExpKey(k);
    lruManager.touch(k);

    if (!dict.find(accessor, k)) return 0;
    auto& val = boost::get<std::deque<std::string>>(accessor->second.value);

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

    if (val.empty()) {
        expirationManager.erase(k);
        dict.erase(accessor);
        currSize.fetch_sub(1);
        lruManager.erase(k);
    }

    return removed;
}

int KVStore::sadd(const std::vector<std::string>& args)
{
    int added = 0;
    const std::string& k = args[0];

    tbb::concurrent_hash_map<std::string, RESPValue>::accessor accessor;
    checkExpKey(k);
    lruManager.touch(k);

    if (!dict.find(accessor, k))
    {
        if (!spaceLeft()) evictTill();
        dict.insert({k,RESPValue{
            storeType::SET,
            std::unordered_set<std::string>(args.begin() + 1, args.end())
            }});
        currSize.fetch_add(1);
        return static_cast<int>(args.size() - 1);
    }
    //else
    auto& val = boost::get<std::unordered_set<std::string>>(accessor->second.value);

    for (auto i = args.begin() + 1; i != args.end(); ++i)
    {
        if (val.insert(*i).second)
        {
            added++;
        }
    }

    return added;
}
int KVStore::srem(const std::vector<std::string>& args)
{
    int removed = 0;
    const std::string& k = args[0];

    tbb::concurrent_hash_map<std::string, RESPValue>::accessor accessor;
    checkExpKey(k);
    lruManager.touch(k);

    if (!dict.find(accessor, k)) return removed;
    auto& val = boost::get<std::unordered_set<std::string>>(accessor->second.value);

    for (auto i = args.begin() + 1; i != args.end(); ++i)
    {
        if (val.erase(*i))
        {
            removed++;
        }
    }

    if (val.empty()) {
        expirationManager.erase(k);
        dict.erase(accessor);
        currSize.fetch_sub(1);
        lruManager.erase(k);
    }

    return removed;
}
bool KVStore::sismember(const std::string& k, const std::string& v)
{
    tbb::concurrent_hash_map<std::string, RESPValue>::accessor accessor;
    checkExpKey(k);
    lruManager.touch(k);

    if (!dict.find(accessor, k)) return false;
    const auto& val = boost::get<std::unordered_set<std::string>>(accessor->second.value);

    return val.contains(v);
}
std::vector<std::optional<std::string>> KVStore::smembers(const std::string& k)
{
    std::vector<std::optional<std::string>> ret{};
    tbb::concurrent_hash_map<std::string, RESPValue>::accessor accessor;
    checkExpKey(k);
    lruManager.touch(k);

    if (!dict.find(accessor, k)) return ret;
    const auto& val = boost::get<std::unordered_set<std::string>>(accessor->second.value);

    for (const auto& i : val)
    {
        ret.emplace_back(i);
    }

    return ret;
}
int KVStore::scard(const std::string& k)
{
    tbb::concurrent_hash_map<std::string, RESPValue>::accessor accessor;
    checkExpKey(k);
    lruManager.touch(k);

    if (!dict.find(accessor, k)) return 0;
    const auto& val = boost::get<std::unordered_set<std::string>>(accessor->second.value);

    return static_cast<int>(val.size());
}
std::vector<std::optional<std::string>> KVStore::spop(const std::string& k, const int& count)
{
    std::vector<std::optional<std::string>> ret{};
    tbb::concurrent_hash_map<std::string, RESPValue>::accessor accessor;
    checkExpKey(k);
    lruManager.touch(k);

    if (!dict.find(accessor, k) || count == 0) return ret;
    auto& val = boost::get<std::unordered_set<std::string>>(accessor->second.value);

    for (int i = 0; i < count && !val.empty(); ++i)
    {
        //TODO make randomized pop
        auto it = val.begin();
        ret.emplace_back(*it);
        val.erase(it);
    }

    if (val.empty()) {
        expirationManager.erase(k);
        dict.erase(accessor);
        currSize.fetch_sub(1);
        lruManager.erase(k);
    }

    return ret;
}

int KVStore::hset(const std::vector<std::string>& args)
{

    int added = 0;
    const std::string& k = args[0];

    tbb::concurrent_hash_map<std::string, RESPValue>::accessor accessor;
    checkExpKey(k);
    lruManager.touch(k);

    if (args.size() < 3 || args.size() % 2 == 0) return false; //just to be cautious, but handle function already handles this

    if (!dict.find(accessor, k))
    {
        if (!spaceLeft()) evictTill();
        std::unordered_map<std::string, std::string> newMap;
        for (auto i = 1; i < args.size(); i += 2)
        {
            newMap[args[i]] = args[i + 1];
            added++;
        }
        dict.insert({k,RESPValue{storeType::HASH, newMap}});
        currSize.fetch_add(1);
        return added;
    }
    //else
    auto& val = boost::get<std::unordered_map<std::string, std::string>>(accessor->second.value);

    for (auto i = 1; i < args.size(); i += 2)
    {
        if (!val.contains(args[i])) added++;
        val[args[i]] = args[i + 1];
    }
    return added;
}
std::optional<std::string> KVStore::hget(const std::string& k, const std::string& f)
{
    tbb::concurrent_hash_map<std::string, RESPValue>::accessor accessor;
    checkExpKey(k);
    lruManager.touch(k);

    if (!dict.find(accessor, k)) return std::nullopt;
    auto& val = boost::get<std::unordered_map<std::string,std::string>>(accessor->second.value);

    const auto foundField = val.find(f);
    if (foundField == val.end()) return std::nullopt;

    return foundField->second;
}
int KVStore::hdel(const std::vector<std::string>& args)
{
    int removed = 0;
    const std::string& k = args[0];

    tbb::concurrent_hash_map<std::string, RESPValue>::accessor accessor;
    checkExpKey(k);
    lruManager.touch(k);

    if (!dict.find(accessor, k)) return removed;
    auto& val = boost::get<std::unordered_map<std::string,std::string>>(accessor->second.value);

    for (auto it = args.begin() + 1; it != args.end(); ++it)
    {
        if (val.erase(*it) > 0)
        {
            removed++;
        }
    }

    if (val.empty()) {
        expirationManager.erase(k);
        dict.erase(accessor);
        currSize.fetch_sub(1);
        lruManager.erase(k);
    }

    return removed;
}
bool KVStore::hexists(const std::string& k, const std::string& f)
{
    tbb::concurrent_hash_map<std::string, RESPValue>::accessor accessor;
    checkExpKey(k);
    lruManager.touch(k);

    if (!dict.find(accessor, k)) return false;
    auto& val = boost::get<std::unordered_map<std::string,std::string>>(accessor->second.value);

    if (const auto foundField = val.find(f); foundField == val.end()) return false;

    return true;
}
int KVStore::hlen(const std::string& k)
{
    tbb::concurrent_hash_map<std::string, RESPValue>::accessor accessor;
    checkExpKey(k);
    lruManager.touch(k);

    if (!dict.find(accessor, k)) return 0;
    const auto& val = boost::get<std::unordered_map<std::string,std::string>>(accessor->second.value);

    return static_cast<int>(val.size());
}
std::vector<std::optional<std::string>> KVStore::hkeys(const std::string& k)
{
    std::vector<std::optional<std::string>> ret{};
    tbb::concurrent_hash_map<std::string, RESPValue>::accessor accessor;
    checkExpKey(k);
    lruManager.touch(k);

    if (!dict.find(accessor, k)) return ret;
    auto& val = boost::get<std::unordered_map<std::string,std::string>>(accessor->second.value);

    for (const auto& i : val)
    {
        ret.emplace_back(std::make_optional(i.first));
    }
    //would use std::sort(ret.begin(), ret.end()) but not needed as redis doesn't do it either (same for hvals)
    return ret;
}
std::vector<std::optional<std::string>> KVStore::hvals(const std::string& k)
{
    std::vector<std::optional<std::string>> ret{};

    tbb::concurrent_hash_map<std::string, RESPValue>::accessor accessor;
    checkExpKey(k);
    lruManager.touch(k);

    if (!dict.find(accessor, k)) return ret;
    const auto& val = boost::get<std::unordered_map<std::string,std::string>>(accessor->second.value);

    for (const auto& i : val)
    {
        ret.emplace_back(std::make_optional(i.second));
    }
    return ret;
}
std::vector<std::optional<std::string>> KVStore::hmget(const std::vector<std::string>& args)
{
    const std::string& k = args[0];
    std::vector<std::optional<std::string>> ret{};

    tbb::concurrent_hash_map<std::string, RESPValue>::accessor accessor;
    checkExpKey(k);
    lruManager.touch(k);

    if (!dict.find(accessor, k))
    {
        ret.resize(args.size() - 1, std::nullopt); //a bit messy but will do?
        return ret;
    }
    //else
    auto& val = boost::get<std::unordered_map<std::string, std::string>>(accessor->second.value);

    for (auto i = 1; i < args.size(); ++i)
    {
        const auto field = val.find(args[i]);

        if (field == val.end()) ret.emplace_back(std::nullopt);
        else ret.emplace_back(std::make_optional(field->second));
    }

    return ret;
}
std::vector<std::optional<std::string>> KVStore::hgetall(const std::string& k)
{
    std::vector<std::optional<std::string>> ret{};

    tbb::concurrent_hash_map<std::string, RESPValue>::accessor accessor;
    checkExpKey(k);
    lruManager.touch(k);

    if (!dict.find(accessor, k)) return ret;

    const auto& val = boost::get<std::unordered_map<std::string, std::string>>(accessor->second.value);
    for (const auto& [field, value] : val)
    {
        ret.emplace_back(field);
        ret.emplace_back(value);
    }

    return ret;
}
