#pragma once

#include <string>
#include <optional>
#include <vector>
#include <tbb/concurrent_hash_map.h>

#include "config.h"
#include "respvalue.hpp"
#include "snapshot.hpp"
#include "expire.hpp"
#include "LRU.hpp"

class KVStore {
public:
    explicit KVStore(bool persist, const std::string& fileName = SAVEFILE_PATH, int maxKeys = KEY_LIMIT);
    ~KVStore();

    //Helpers
    std::optional<storeType> getType(const std::string& k); //Gets storeType of value
    std::optional<std::string> checkTypeError(const std::string& k, storeType expected);
    void checkExpKey(const std::string& k);
    bool spaceLeft() const;
    void evictTill();

    //Persistence
    void loadFromDisk();
    void saveToDisk();

    //Basics
    int del(const std::vector<std::string>& args);
    int exists(const std::vector<std::string>& args);
    void flushall();

    //Strings
    bool set(const std::string& k, const std::string& v);
    std::optional<std::string> get(const std::string& k);
    std::optional<int> incr(const std::string& k);
    std::optional<int> dcr(const std::string& k);
    std::optional<int> incrby(const std::string& k, const int& count);
    std::optional<int> dcrby(const std::string& k, const int& count);
    std::vector<std::optional<std::string>> mget(const std::vector<std::string>& args);
    int append(const std::string& k, const std::string& v);

    //TTL
    bool expire(const std::string& k, int s);
    int ttl(const std::string& k);
    bool persist(const std::string& k);

    //Lists
    int lpush(const std::vector<std::string>& args);
    int rpush(const std::vector<std::string>& args);
    std::optional<std::string> lpop(const std::string& k);
    std::optional<std::string> rpop(const std::string& k);
    std::vector<std::optional<std::string>> lrange(const std::string& k, const int& start, const int& stop);
    int llen(const std::string& k);
    std::optional<std::string> lindex(const std::string& k, const int& index);
    bool lset(const std::string& k, const int& index, const std::string& v);
    int lrem(const std::string& k, const int& count, const std::string& v);

    //Sets
    int sadd(const std::vector<std::string>& args);
    int srem(const std::vector<std::string>& args);
    bool sismember(const std::string& k, const std::string& v);
    std::vector<std::optional<std::string>> smembers(const std::string& k);
    int scard(const std::string& k);
    std::vector<std::optional<std::string>> spop(const std::string& k, const int& count);

    //Hashes
    int hset(const std::vector<std::string>& args);
    std::optional<std::string> hget(const std::string& k, const std::string& f);
    int hdel(const std::vector<std::string>& args);
    bool hexists(const std::string& k, const std::string& f);
    int hlen(const std::string& k);
    std::vector<std::optional<std::string>> hkeys(const std::string& k);
    std::vector<std::optional<std::string>> hvals(const std::string& k);
    std::vector<std::optional<std::string>> hmget(const std::vector<std::string>& args);
    std::vector<std::optional<std::string>> hgetall(const std::string& k);

private:
    bool persistenceToggle; //Originally for testing
    tbb::concurrent_hash_map<std::string,RESPValue> dict; //Main store
    Expiration expirationManager; //For key ttl handling
    Snapshot snapshotManager; //Persistence
    LRU lruManager; //Eviction on max limit reach

    //Both sizes are just key count
    size_t maxSize = KEY_LIMIT; //config.h
    std::atomic<size_t> currSize{0};

};
