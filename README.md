# Redis-Clone
Making a Redis clone from scratch using C++. Inspired by the build your own x repo.

## Features
- **Concurrent** TCP connection with **RESP protocol** (Thread pool with Boost)
- In-memory key-value store **(Strings, Lists, Sets, Hashes)**
- **Commands supported**
    - Basic: PING, ECHO, DEL, EXISTS, FLUSHALL
    - String: SET, GET, INCR, DCR, INCRBY, DCRBY, MGET, APPEND
    - Key expiration: EXPIRE, TTL, PERSIST
    - List: LPUSH, RPUSH, LPOP, RPOP, LRANGE, LLEN, LINDEX, LSET, LREM
    - Set: SADD, SREM, SISMEMBER, SMEMBERS, SCARD, SPOP
    - Hash: HSET, HGET, HDEL, HEXISTS, HLEN, HKEYS, HVALS, HMGET, HGETALL
    - Pub/Sub: PUBLISH, SUBSCRIBE, UNSUBSCRIBE
    - Transaction: MULTI, EXEC, DISCARD
- **Key expiration**
- **Pub/Sub** support
- **Transaction** support command queueing
- **LRU eviction** for memory management
- **Snapshotting** with Boost binary serialization (Persistence)
- Thread-safe access with **fine-grained locking** with TBB
- **Catch2 unit testing** with CI workflows

## Demo
TBD

## Build

### Prerequisites
- C++20 compiler
- Run on WSL or Linux
- [TBB (oneAPI Threading Building Blocks)](https://github.com/oneapi-src/oneTBB)
- [Boost](https://www.boost.org/) (serialization, asio)
- CMake
- Catch2 testing

### Steps to build
```sh
git clone https://github.com/BHalacsy/Redis-Clone
cd Redis-Clone
mkdir build && cd build
cmake .
cmake --build .
```

### Run
```./redis_clone```

### Usage
For specifics on various implemented redis commands I recommend looking at the [redis docs](https://redis.io/docs/latest/commands) for what you may be looking for.
