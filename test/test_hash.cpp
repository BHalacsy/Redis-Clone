#define CATCH_CONFIG_MAIN

#include <set>
#include <thread>
#include <catch2/catch_test_macros.hpp>

#include "parser.hpp"
#include "kvstore.hpp"
#include "commands.hpp"
#include "util.hpp"


TEST_CASE("HSET and HGET methods", "[hset/hget][kvstore method][unit]")
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

TEST_CASE("HDEL and HEXISTS methods", "[hdel/hexists][kvstore method][unit]")
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

TEST_CASE("HLEN, HKEYS, HVALS methods", "[hlen/hkeys/hvals][kvstore method][unit]")
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

TEST_CASE("HMGET methods", "[hmget][kvstore method][unit]")
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