#pragma once

#include <unordered_map>
#include <chrono>
#include <string>
#include <optional>
#include <mutex>
#include <vector>

#include "config.h"
#include "RESPtype.hpp"
#include "snapshot.hpp"
#include "expire.hpp"

class KVStore {
public:
    explicit KVStore(bool persist, const std::string& fileName = SAVEFILE_PATH);
    ~KVStore();

    void removeExp(const std::string& k);
    void removeExpAll();
    std::optional<storeType> getType(const std::string& k);
    void loadFromDisk();
    void saveToDisk();


    bool set(const std::string& k, const std::string& v);
    std::optional<std::string> get(const std::string& k);
    int del(const std::vector<std::string>& args);
    int exists(const std::vector<std::string>& args);
    std::optional<int> incr(const std::string& k);
    std::optional<int> dcr(const std::string& k);
    bool expire(const std::string& k, int s);
    int ttl(const std::string& k);
    void flushall();
    std::vector<std::optional<std::string>> mget(const std::vector<std::string>& args);

    int lpush(const std::vector<std::string>& args);
    int rpush(const std::vector<std::string>& args);
    std::optional<std::string> lpop(const std::string& k);
    std::optional<std::string> rpop(const std::string& k);
    std::vector<std::optional<std::string>> lrange(const std::string& k, const int& start, const int& stop);
    int llen(const std::string& k);
    std::optional<std::string> lindex(const std::string& k, const int& index);
    bool lset(const std::string& k, const int& index, const std::string& v);
    int lrem(const std::string& k, const int& count, const std::string& v);

    int sadd(const std::vector<std::string>& args);
    int srem(const std::vector<std::string>& args);
    bool sismember(const std::string& k, const std::string& v);
    std::vector<std::optional<std::string>> smembers(const std::string& k);
    int scard(const std::string& k);
    std::vector<std::optional<std::string>> spop(const std::string& k, const int& count);

    int hset(const std::vector<std::string>& args);
    std::optional<std::string> hget(const std::string& k, const std::string& f);
    int hdel(const std::vector<std::string>& args);
    bool hexists(const std::string& k, const std::string& f);
    int hlen(const std::string& k);
    std::vector<std::optional<std::string>> hkeys(const std::string& k);
    std::vector<std::optional<std::string>> hvals(const std::string& k);
    std::vector<std::optional<std::string>> hmget(const std::vector<std::string>& args);


private:
    bool persistenceToggle; //toggle to not persist when testing
    std::unordered_map<std::string,RESPValue> dict; //main store
    Expiration expirationManager;
    Snapshot snapshotManager;

    std::mutex mtx; //thread lock for expTable
};
