#define CATCH_CONFIG_MAIN

#include <set>
#include <thread>

#include "parser.hpp"
#include "kvstore.hpp"
#include "commands.hpp"
#include "util.hpp"
#include <catch2/catch_test_macros.hpp>


TEST_CASE("Lpush and Rpush methods", "[lpush/rpush][kvstore method][unit]")
{
    KVStore kv(false);

    SECTION("Lpush and Rpush expected")
    {
        REQUIRE(kv.lpush({"mylist", "a"}) == 1);
        REQUIRE(kv.lpush({"mylist", "b"}) == 2);
        REQUIRE(kv.rpush({"mylist", "c"}) == 3);
        REQUIRE(kv.rpush({"mylist", "d"}) == 4);

        auto range = kv.lrange("mylist", 0, -1);
        REQUIRE(range.size() == 4);
        REQUIRE(range[0] == "b");
        REQUIRE(range[1] == "a");
        REQUIRE(range[2] == "c");
        REQUIRE(range[3] == "d");
    }
}
TEST_CASE("Lpop and Rpop methods", "[lpop/rpop][kvstore method][unit]")
{
    KVStore kv(false);

    SECTION("Lpop and Rpop expected")
    {
        kv.rpush({"mylist", "a"});
        kv.rpush({"mylist", "b"});
        kv.rpush({"mylist", "c"});
        REQUIRE(kv.lpop("mylist").value() == "a");
        REQUIRE(kv.rpop("mylist").value() == "c");
        REQUIRE(kv.lpop("mylist").value() == "b");
    }

    SECTION("Pop empty list")
    {
        REQUIRE(kv.lpop("mylist") == std::nullopt);
    }
}
TEST_CASE("Lrange method", "[lrange][kvstore method][unit]")
{
    KVStore kv(false);

    SECTION("lrange expected")
    {
        kv.rpush({"mylist", "a"});
        kv.rpush({"mylist", "b"});
        kv.rpush({"mylist", "c"});
        REQUIRE(kv.llen("mylist") == 3);
        kv.lpop("mylist");
        REQUIRE(kv.llen("mylist") == 2);

    }

    SECTION("lrange empty list")
    {
        kv.lpop("mylist");
        kv.lpop("mylist");
        REQUIRE(kv.llen("mylist") == 0);
    }
}
TEST_CASE("Llen method", "[llen][kvstore method][unit]")
{
    KVStore kv(false);

    SECTION("Llen expected")
    {
        kv.rpush({"mylist", "a"});
        kv.rpush({"mylist", "b"});
        kv.rpush({"mylist", "c"});
        REQUIRE(kv.llen("mylist") == 3);
        kv.lpop("mylist");
        REQUIRE(kv.llen("mylist") == 2);

    }

    SECTION("Llen empty list")
    {
        kv.lpop("mylist");
        kv.lpop("mylist");
        REQUIRE(kv.llen("mylist") == 0);
    }
}
TEST_CASE("Lindex method", "[lindex][kvstore method][unit]")
{
    KVStore kv(false);

    SECTION("Lindex expected")
    {
        kv.rpush({"mylist", "a"});
        kv.rpush({"mylist", "b"});
        kv.rpush({"mylist", "c"});
        REQUIRE(kv.lindex("mylist", 0) == "a");
        REQUIRE(kv.lindex("mylist", 2) == "c");
    }

    SECTION("Lindex non value empty in list")
    {
        REQUIRE(kv.lindex("mylist", 3) == std::nullopt);
    }
}
TEST_CASE("Lset method", "[lset][kvstore method][unit]")
{
    KVStore kv(false);

    SECTION("Lset expected")
    {
        kv.rpush({"mylist", "a"});
        kv.rpush({"mylist", "b"});
        kv.rpush({"mylist", "c"});
        REQUIRE(kv.lset("mylist", 1, "z"));
        REQUIRE(kv.lindex("mylist", 1) == "z");
        REQUIRE(kv.lset("mylist", 2, "x"));
        REQUIRE(kv.lindex("mylist", 2) == "x");
    }

    SECTION("Lset non value in list")
    {
        REQUIRE(!kv.lset("mylist", 5, "x"));
    }
}
TEST_CASE("Lrem method", "[lrem][kvstore method][unit]")
{
    KVStore kv(false);

    SECTION("Lrem expected")
    {
        kv.rpush({"mylist", "a"});
        kv.rpush({"mylist", "a"});
        kv.rpush({"mylist", "a"});
        kv.rpush({"mylist", "b"});
        kv.rpush({"mylist", "b"});
        REQUIRE(kv.lrem("mylist", 1, "a") == 1);
        REQUIRE(kv.lrem("mylist", 0, "a") == 2);
        REQUIRE(kv.lindex("mylist", 0) == "b");
    }

    SECTION("Lrem non value in list")
    {
        REQUIRE(kv.lrem("mylist", 0, "c") == 0);
        REQUIRE(kv.lrem("otherlist", 0, "c") == 0);
    }
}
TEST_CASE("Sadd method", "[sadd][kvstore method][unit]")
{
    KVStore kv(false);

    SECTION("Sadd expected and add existing")
    {
        REQUIRE(kv.sadd({"myset", "a", "b", "c"}) == 3);
        REQUIRE(kv.sadd({"myset", "b", "d"}) == 1);
        REQUIRE(kv.sadd({"myset", "b", "d"}) == 0);
    }
}
TEST_CASE("Srem method", "[srem][kvstore method][unit]")
{
    KVStore kv(false);
    kv.sadd({"myset", "a", "b", "c"});

    SECTION("Srem expected")
    {
        REQUIRE(kv.srem({"myset", "b", "x"}) == 1);
        REQUIRE(kv.sismember("myset", "b") == false);
    }

    SECTION("Srem non-existing set")
    {
        REQUIRE(kv.srem({"otherset", "a", "c"}) == 0);
    }
}

TEST_CASE("Sismember method", "[sismember][kvstore method][unit]")
{
    KVStore kv(false);
    kv.sadd({"myset", "a", "b", "c"});

    SECTION("Sismember expected")
    {
        REQUIRE(kv.sismember("myset", "b") == true);
        REQUIRE(kv.sismember("myset", "x") == false);

    }

    SECTION("Sismember non-existing set")
    {
        REQUIRE(kv.sismember("otherset", "a") == false);
    }
}
TEST_CASE("Smembers method", "[Smembers][kvstore method][unit]")
{
    KVStore kv(false);
    kv.sadd({"myset", "a", "b", "c"});

    SECTION("Smembers expected")
    {
        auto members = kv.smembers("myset");
        std::set<std::string> res;
        for (auto i : members) if (i) res.insert(*i);
        REQUIRE(res == std::set<std::string>{"a", "b", "c"});
    }

    SECTION("Smembers non-existing set")
    {
        REQUIRE(kv.smembers("otherset").empty());
    }
}
TEST_CASE("Scard method", "[Scard][kvstore method][unit]")
{
    KVStore kv(false);
    kv.sadd({"myset", "a", "b", "c"});

    SECTION("Scard expected")
    {
        REQUIRE(kv.scard("myset") == 3);
        kv.srem({"myset", "a"});
        REQUIRE(kv.scard("myset") == 2);
    }

    SECTION("Scard non-existing set")
    {
        REQUIRE(kv.scard("otherset") == 0);
    }
}
TEST_CASE("Spops method", "[spop][kvstore method][unit]")
{
    KVStore kv(false);
    kv.sadd({"myset", "a", "b", "c"});

    SECTION("Spop expected")
    {
        auto popped = kv.spop("myset", 2);
        REQUIRE(popped.size() == 2);
        REQUIRE(kv.scard("myset") == 1);
    }

    SECTION("Spop non-existing set")
    {
        auto popped = kv.spop("otherset", 2);
        REQUIRE(popped.empty());
    }
}

TEST_CASE("Hset and Hget methods", "[hset/hget][kvstore method][unit]")
{
    KVStore kv(false);

    SECTION("Hset and Hget new elements expected and overwrite field")
    {
        REQUIRE(kv.hset({"myhash", "f1", "v1"}) == 1);
        REQUIRE(kv.hget("myhash", "f1") == "v1");
        REQUIRE(kv.hset({"myhash", "f1", "v2"}) == 0);
        REQUIRE(kv.hget("myhash", "f1") == "v2");
    }

    SECTION("HsetHget empty key and empty field list")
    {
        REQUIRE(kv.hget("otherhash", "f1") == std::nullopt);
        REQUIRE(kv.hget("myhash", "otherfield") == std::nullopt);
    }
}
TEST_CASE("Hdel and Hexists methods", "[hdel/hexists][kvstore method][unit]")
{
    KVStore kv(false);
    kv.hset({"myhash", "f1", "v1", "f2", "v2"});

    SECTION("Hdel and Hexists new elements expected")
    {
        REQUIRE(kv.hdel({"myhash", "f1"}) == 1);
        REQUIRE(kv.hexists("myhash", "f1") == false);
        REQUIRE(kv.hexists("myhash", "f2") == true);
    }

    SECTION("Hdel non-existing field") {
        REQUIRE(kv.hdel({"myhash", "f3"}) == 0);
    }
}
TEST_CASE("Hlen, Hkeys, Hvals methods", "[hlen/hkeys/hvals][kvstore method][unit]")
{
    KVStore kv(false);
    kv.hset({"myhash", "f1", "v1", "f2", "v2", "f3", "v3"});

    SECTION("Hlen expected")
    {
        REQUIRE(kv.hlen("myhash") == 3);
    }

    SECTION("Hkeys returns all fields") {
        auto keys = kv.hkeys("myhash");
        std::set<std::string> expected = {"f1", "f2", "f3"};
        std::set<std::string> actual;
        for (const auto& key : keys) {
            if (key) {
                actual.insert(*key);
            }
        }
        REQUIRE(actual == expected);
    }

    SECTION("Hvals returns all values") {
        auto vals = kv.hvals("myhash");
        std::set<std::string> expected = {"v1", "v2", "v3"};
        std::set<std::string> actual;
        for (const auto& val : vals) {
            if (val) {
                actual.insert(*val);
            }
        }
        REQUIRE(actual == expected);
    }
}
TEST_CASE("Hmget methods", "[hmget][kvstore method][unit]")
{
    KVStore kv(false);
    kv.hset({"myhash", "f1", "v1", "f2", "v2", "f3", "v3"});
    SECTION("Hmget expected")
    {
        auto vals = kv.hmget({"myhash", "f1", "f2", "f3", "f4"});
        REQUIRE(vals.size() == 4);
        REQUIRE(vals[0] == "v1");
        REQUIRE(vals[1] == "v2");
        REQUIRE(vals[2] == "v3");
        REQUIRE(vals[3] == std::nullopt);
    }

    SECTION("Hmget on non-existing hash")
    {
        auto vals = kv.hmget({"nohash", "f1", "f2"});
        REQUIRE(vals.size() == 2);
        REQUIRE(vals[0] == std::nullopt);
        REQUIRE(vals[1] == std::nullopt);
    }
}


TEST_CASE("LPUSH and RPUSH commands", "[lpush/rpush][command handler][unit]")
{
    KVStore kv(false);
    kv.lpush({"mylist", "a", "b"});

    SECTION("LPUSH and RPUSH expected")
    {
        REQUIRE(handleLPUSH(kv, {"mylist", "x"}) == ":3\r\n");
        REQUIRE(handleRPUSH(kv, {"mylist", "y"}) == ":4\r\n");
        REQUIRE(kv.lindex("mylist", 0) == "x");
        REQUIRE(kv.lindex("mylist", 3) == "y");
    }

    SECTION("LPUSH and RPUSH bad args")
    {
        REQUIRE(handleLPUSH(kv, {}) == argumentError("1 or more", 0));
        REQUIRE(handleRPUSH(kv, {}) == argumentError("1 or more", 0));
    }
}
TEST_CASE("LPOP and RPOP commands", "[lpop/rpop][command handler][unit]")
{
    KVStore kv(false);
    kv.rpush({"mylist", "a", "b"});

    SECTION("LPOP and RPOP expected")
    {
        REQUIRE(handleLPOP(kv, {"mylist"}) == "$1\r\na\r\n");
        REQUIRE(handleRPOP(kv, {"mylist"}) == "$1\r\nb\r\n");
        REQUIRE(handleLPOP(kv, {"mylist"}) == "$-1\r\n");
    }

    SECTION("LPOP and RPOP bad args")
    {
        REQUIRE(handleLPOP(kv, {}) == argumentError("1", 0));
        REQUIRE(handleRPOP(kv, {"mylist", "a", "b"}) == argumentError("1", 3));
    }
}
TEST_CASE("LRANGE command", "[lrange][command handler][unit]")
{
    KVStore kv(false);
    kv.rpush({"mylist", "a", "b", "c"});
    SECTION("LRANGE expected")
    {
        REQUIRE(handleLRANGE(kv, {"mylist", "0", "-1"}) == "*3\r\n$1\r\na\r\n$1\r\nb\r\n$1\r\nc\r\n");
        REQUIRE(handleLRANGE(kv, {"mylist", "1", "1"}) == "*1\r\n$1\r\nb\r\n");
        REQUIRE(handleLRANGE(kv, {"mylist", "5", "10"}) == "*0\r\n");
    }

    SECTION("LRANGE bad args")
    {
        REQUIRE(handleLRANGE(kv, {"mylist"}) == argumentError("3", 1));
        REQUIRE(handleLRANGE(kv, {"mylist", "b", "1"}) == "-ERR value is not an integer or out of range\r\n");
    }
}
TEST_CASE("LLEN command", "[llen][command handler][unit]")
{
    KVStore kv(false);
    kv.rpush({"mylist", "a", "b"});
    SECTION("Llen expected")
    {

        REQUIRE(handleLLEN(kv, {"mylist"}) == ":2\r\n");
        REQUIRE(handleLLEN(kv, {"otherlist"}) == ":0\r\n");
    }

    SECTION("LLEN bad args")
    {
        REQUIRE(handleLLEN(kv, {"mylist", "b"}) == argumentError("1", 2));
    }
}
TEST_CASE("LINDEX command", "[lindex][command handler][unit]")
{
    KVStore kv(false);
    kv.rpush({"mylist", "a", "b"});
    SECTION("Lindex expected")
    {
        REQUIRE(handleLINDEX(kv, {"mylist", "1"}) == "$1\r\nb\r\n");
        REQUIRE(handleLINDEX(kv, {"mylist", "2"}) == "$-1\r\n");
    }

    SECTION("LINDEX bad args")
    {
        REQUIRE(handleLINDEX(kv, {"mylist"}) == argumentError("2", 1));
    }
}
TEST_CASE("LSET command", "[lset][command handler][unit]")
{
    KVStore kv(false);
    kv.rpush({"mylist", "a", "b"});
    SECTION("LSET expected")
    {

        REQUIRE(handleLSET(kv, {"mylist", "1", "z"}) == "+OK\r\n");
        REQUIRE(kv.rpop("mylist") == "z");
        REQUIRE(handleLSET(kv, {"mylist", "5", "x"}) == "-ERR no such key or value out of range\r\n");
    }

    SECTION("LSET bad args")
    {
        REQUIRE(handleLSET(kv, {"mylist", "b", "y"}) == "-ERR value is not an integer or out of range\r\n");
        REQUIRE(handleLSET(kv, {"mylist"}) == argumentError("3", 1));
    }
}
TEST_CASE("LREM command", "[lrem][command handler][unit]")
{
    KVStore kv(false);
    kv.rpush({"mylist", "a", "b", "a", "a", "a"});
    SECTION("LREM expected")
    {
        REQUIRE(handleLREM(kv, {"mylist", "2", "a"}) == ":2\r\n");
        REQUIRE(handleLREM(kv, {"mylist", "0", "a"}) == ":2\r\n");
        REQUIRE(handleLREM(kv, {"mylist", "0", "x"}) == ":0\r\n");
    }

    SECTION("LREM bad args")
    {
        REQUIRE(handleLREM(kv, {"mylist", "b", "y"}) == "-ERR value is not an integer or out of range\r\n");
        REQUIRE(handleLREM(kv, {"mylist"}) == argumentError("3", 1));
    }
}

TEST_CASE("SADD command", "[sadd][command handler][unit]")
{
    KVStore kv(false);

    SECTION("SADD expected")
    {
        REQUIRE(handleSADD(kv, {"myset", "a", "b", "c"}) == ":3\r\n");
        REQUIRE(handleSADD(kv, {"myset", "b", "d"}) == ":1\r\n");
    }

    SECTION("SADD bad args")
    {
        REQUIRE(handleSADD(kv, {"myset"}) == argumentError("2 or more", 1));
        REQUIRE(handleSADD(kv, {}) == argumentError("2 or more", 0));
    }
}
TEST_CASE("SREM command", "[srem][command handler][unit]")
{
    KVStore kv(false);
    kv.sadd({"myset", "a", "b", "c"});

    SECTION("SREM expected")
    {
        REQUIRE(handleSREM(kv, {"myset", "b", "x"}) == ":1\r\n");
        REQUIRE(handleSREM(kv, {"myset", "a", "c"}) == ":2\r\n");
    }

    SECTION("SREM bad args")
    {
        REQUIRE(handleSREM(kv, {"myset"}) == argumentError("2 or more", 1));
        REQUIRE(handleSREM(kv, {}) == argumentError("2 or more", 0));
    }
}
TEST_CASE("SISMEMBER command", "[sismember][command handler][unit]")
{
    KVStore kv(false);
    kv.sadd({"myset", "a", "b", "c"});

    SECTION("SISMEMBER expected")
    {
        REQUIRE(handleSISMEMBER(kv, {"myset", "b"}) == ":1\r\n");
        REQUIRE(handleSISMEMBER(kv, {"myset", "x"}) == ":0\r\n");
    }

    SECTION("SISMEMBER bad args")
    {
        REQUIRE(handleSISMEMBER(kv, {"myset"}) == argumentError("2", 1));
        REQUIRE(handleSISMEMBER(kv, {}) == argumentError("2", 0));
    }
}
TEST_CASE("SMEMBERS command", "[smembers][command handler][unit]")
{
    KVStore kv(false);
    kv.sadd({"myset", "c", "bx", "a"});

    SECTION("SMEMBERS expected")
    {
        REQUIRE(handleSMEMBERS(kv, {"myset"}) == "*3\r\n$1\r\na\r\n$2\r\nbx\r\n$1\r\nc\r\n");
    }

    SECTION("SMEMBERS non-existing set")
    {
        REQUIRE(handleSMEMBERS(kv, {"otherset"}) == "*0\r\n");
    }

    SECTION("SMEMBERS bad args")
    {
        REQUIRE(handleSMEMBERS(kv, {}) == argumentError("1", 0));
        REQUIRE(handleSMEMBERS(kv, {"myset", "more"}) == argumentError("1", 2));
    }
}
TEST_CASE("SCARD command", "[scard][command handler][unit]")
{
    KVStore kv(false);
    kv.sadd({"myset", "a", "b", "c"});

    SECTION("SCARD expected")
    {
        REQUIRE(handleSCARD(kv, {"myset"}) == ":3\r\n");
        kv.srem({"myset", "a"});
        REQUIRE(handleSCARD(kv, {"myset"}) == ":2\r\n");
    }

    SECTION("SCARD non-existing set")
    {
        REQUIRE(handleSCARD(kv, {"otherset"}) == ":0\r\n");
    }

    SECTION("SCARD bad args")
    {
        REQUIRE(handleSCARD(kv, {}) == argumentError("1", 0));
        REQUIRE(handleSCARD(kv, {"myset", "more"}) == argumentError("1", 2));
    }
}
TEST_CASE("SPOP command", "[spop][command handler][unit]")
{
    //Didn't do multi pop because its random
    KVStore kv(false);
    kv.sadd({"myset", "a", "b"});

    SECTION("SPOP expected")
    {
        auto popped = handleSPOP(kv, {"myset"});

        REQUIRE((popped == "$1\r\na\r\n" || popped == "$1\r\nb\r\n"));
        REQUIRE(kv.scard("myset") == 1);
    }

    SECTION("SPOP non-existing set")
    {
        REQUIRE(handleSPOP(kv, {"nonexists"}) == "$-1\r\n");
    }

    SECTION("SPOP bad args")
    {
        REQUIRE(handleSPOP(kv, {"myset", "2", "more"}) == argumentError("1 or 2", 3));
        REQUIRE(handleSPOP(kv, {}) == argumentError("1 or 2", 0));
        REQUIRE(handleSPOP(kv, {"myset", "notanumber"}) == "-ERR value is not an integer or out of range\r\n");
    }
}

TEST_CASE("HSET and HGET commands", "[hset/hget][command handler][unit]")
{
    KVStore kv(false);
    SECTION("HSET and HGET expected")
    {
        REQUIRE(handleHSET(kv, {"myhash", "f1", "v1"}) == ":1\r\n");
        REQUIRE(handleHSET(kv, {"myhash", "f1", "v2"}) == ":0\r\n");
        REQUIRE(handleHGET(kv, {"myhash", "f1"}) == "$2\r\nv2\r\n");
        REQUIRE(handleHGET(kv, {"myhash", "f2"}) == "$-1\r\n");
    }

    SECTION("HSET and HGET bad args")
    {
        REQUIRE(handleHSET(kv, {"myhash", "f1"}) == argumentError("3 or more", 2));\
        REQUIRE(handleHSET(kv, {"myhash", "f1", "v2", "f2"}) == "-ERR expected pair of fields and values");
        REQUIRE(handleHSET(kv, {}) == argumentError("3 or more", 0));
        REQUIRE(handleHGET(kv, {"myhash"}) == argumentError("2", 1));
        REQUIRE(handleHGET(kv, {}) == argumentError("2", 0));
    }
}
TEST_CASE("HDEL and HEXISTS commands", "[hdel/hexists][command handler][unit]")
{
    KVStore kv(false);
    kv.hset({"myhash", "f1", "v1", "f2", "v2"});
    SECTION("HDEL and HEXISTS expected")
    {
        REQUIRE(handleHEXISTS(kv, {"myhash", "f1"}) == ":1\r\n");
        REQUIRE(handleHDEL(kv, {"myhash", "f1", "f2"}) == ":2\r\n");
        REQUIRE(handleHDEL(kv, {"myhash", "f3"}) == ":0\r\n");
        REQUIRE(handleHEXISTS(kv, {"myhash", "f1"}) == ":0\r\n");
    }

    SECTION("HDEL and HEXISTS bad args")
    {
        REQUIRE(handleHDEL(kv, {"myhash"}) == argumentError("2 or more", 1));
        REQUIRE(handleHDEL(kv, {}) == argumentError("2 or more", 0));
        REQUIRE(handleHEXISTS(kv, {"myhash"}) == argumentError("2", 1));
        REQUIRE(handleHEXISTS(kv, {}) == argumentError("2", 0));
    }
}
TEST_CASE("HLEN command", "[hlen][command handler][unit]")
{
    KVStore kv(false);
    kv.hset({"myhash", "f1", "v1", "f2", "v2"});

    SECTION("HLEN expected")
    {
        REQUIRE(handleHLEN(kv, {"myhash"}) == ":2\r\n");
        REQUIRE(handleHLEN(kv, {"otherhash"}) == ":0\r\n");
    }

    SECTION("HLEN bad args")
    {
        REQUIRE(handleHLEN(kv, {}) == argumentError("1", 0));
        REQUIRE(handleHLEN(kv, {"myhash", "more"}) == argumentError("1", 2));
    }
}
TEST_CASE("HKEYS and HVALS commands", "[hkeys/hvals][command handler][unit]")
{
    KVStore kv(false);
    kv.hset({"myhash", "f1", "v1", "f2", "v2"});

    SECTION("HKEYS and HVALS expected")
    {
        REQUIRE(handleHKEYS(kv, {"myhash"}) == "*2\r\n$2\r\nf2\r\n$2\r\nf1\r\n");
        REQUIRE(handleHKEYS(kv, {"otherhash"}) == "*0\r\n");
        REQUIRE(handleHVALS(kv, {"myhash"}) == "*2\r\n$2\r\nv2\r\n$2\r\nv1\r\n");
        REQUIRE(handleHVALS(kv, {"otherhash"}) == "*0\r\n");
    }

    SECTION("HKEYS and HVALS bad args")
    {
        REQUIRE(handleHKEYS(kv, {}) == argumentError("1", 0));
        REQUIRE(handleHKEYS(kv, {"myhash", "more"}) == argumentError("1", 2));
        REQUIRE(handleHVALS(kv, {}) == argumentError("1", 0));
        REQUIRE(handleHVALS(kv, {"myhash", "more"}) == argumentError("1", 2));
    }
}
TEST_CASE("HMGET commands", "[hmget][command handler][unit]")
{
    KVStore kv(false);
    kv.hset({"myhash", "f1", "v1", "f2", "v2"});

    SECTION("HMGET expected")
    {
        REQUIRE(handleHMGET(kv, {"myhash", "f1", "f2", "f3"}) == "*3\r\n$2\r\nv1\r\n$2\r\nv2\r\n$-1\r\n");
    }

    SECTION("HMGET bad args")
    {
        REQUIRE(handleHMGET(kv, {"myhash"}) == argumentError("2 or more", 1));
        REQUIRE(handleHMGET(kv, {}) == argumentError("2 or more", 0));
    }
}

TEST_CASE("Persistence", "[persistence][unit]")
{

    const std::string filename = "test.rdb";
    {
        KVStore kv(true, filename);
        kv.set("a", "1");
        kv.lpush({"b", "1"});
        kv.hset({"c", "d", "1"});
    }
    //moves to outer scope and destroys the kv
    std::cout << "hit persist test transfer" << std::endl;
    {
        KVStore kv(true, filename);
        REQUIRE(kv.get("a") == "1");
        REQUIRE(kv.lpop("b") == "1");
        REQUIRE(kv.hget("c", "d") == "1");
    }
}