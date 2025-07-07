#define CATCH_CONFIG_MAIN

#include <thread>
#include <catch2/catch_test_macros.hpp>

#include "parser.hpp"
#include "kvstore.hpp"
#include "commands.hpp"
#include "util.hpp"


TEST_CASE("SET method", "[set][kvstore method][unit]")
{
    KVStore kv(false);
    SECTION("Set new key")
    {
        kv.set("key", "value");
        REQUIRE(kv.get("key") == "value");

        SECTION("Set existing key")
        {
            kv.set("key", "new value");
            REQUIRE(kv.get("key") == "new value");
        }
    }

    SECTION("Set empty key")
    {
        kv.set("", "empty value");
        REQUIRE(kv.get("") == "empty value");
    }

    SECTION("Set empty value")
    {
        kv.set("empty key", "");
        REQUIRE(kv.get("empty key") == "");
    }
}
TEST_CASE("SET command", "[set][command handler][unit]")
{
    KVStore kv(false);
    SECTION("SET expected")
    {
        REQUIRE(handleSET(kv, {"key","value"}) == "+OK\r\n");
        REQUIRE(kv.get("key") == "value");
    }
    SECTION("SET bad args")
    {
        REQUIRE(handleSET(kv, {"key2", "value2", "store"}) == argumentError("2", 3));
        REQUIRE(kv.get("key2") == std::nullopt);
    }
}

TEST_CASE("GET method", "[get][kvstore method][unit]")
{
    KVStore kv(false);
    kv.set("a", "1");
    kv.set("b", "2");

    SECTION("Get existing key")
    {
        REQUIRE(kv.get("a") == "1");
        REQUIRE(kv.get("b") == "2");
    }

    SECTION("Get a non-existing key")
    {
        REQUIRE(kv.get("c") == std::nullopt);
    }
}
TEST_CASE("GET command", "[get][command handler][unit]")
{
    KVStore kv(false);
    SECTION("GET expected")
    {
        kv.set("a", "value");
        REQUIRE(handleGET(kv, {"a"}) == "$5\r\nvalue\r\n");
    }

    SECTION("GET non-existing")
    {
        REQUIRE(handleGET(kv, {"f"}) == "$-1\r\n");
    }

    SECTION("GET bad args")
    {
        REQUIRE(handleGET(kv, {"b", "c"}) == argumentError("1", 2));
    }
}

TEST_CASE("INCR method", "[incr][kvstore method][unit]")
{
    KVStore kv(false);
    kv.set("b", "value");

    kv.incr("a");
    SECTION("Make new key and incr")
    {
        REQUIRE(kv.get({"a"}) == "1");
    }

    kv.incr("a");
    SECTION("Incr existing key")
    {
        REQUIRE(kv.get({"a"}) == "2");
    }

    SECTION("Incr non-number")
    {
        REQUIRE(kv.incr({"b"}) == std::nullopt);
    }
}
TEST_CASE("INCR command", "[incr][command handler][unit]")
{
    KVStore kv(false);
    kv.set("b", "5");
    kv.set("c", "nonnum");
    SECTION("INCR expected")
    {
        REQUIRE(handleINCR(kv, {"a"}) == ":1\r\n");
        REQUIRE(handleINCR(kv, {"a"}) == ":2\r\n");
        REQUIRE(handleINCR(kv, {"b"}) == ":6\r\n");
        REQUIRE(handleINCR(kv, {"c"}) == "-ERR value is not number or out of range\r\n");
    }

    SECTION("INCR bad args")
    {
        REQUIRE(handleINCR(kv, {"c", "d"}) == argumentError("1", 2));
    }
}

TEST_CASE("DCR method", "[dcr][kvstore method][unit]")
{
    KVStore kv(false);
    kv.set("b", "value");

    kv.dcr("a");
    SECTION("Make new key and dcr")
    {
        REQUIRE(kv.get({"a"}) == "-1");
    }

    kv.dcr("a");
    SECTION("Dcr existing key")
    {
        REQUIRE(kv.get({"a"}) == "-2");
    }

    SECTION("Dcr non-number")
    {
        REQUIRE(kv.dcr({"b"}) == std::nullopt);
    }
}
TEST_CASE("DCR command", "[dcr][command handler][unit]")
{
    KVStore kv(false);
    kv.set("b", "5");
    kv.set("c", "nonnum");
    SECTION("DCR expected")
    {
        REQUIRE(handleDCR(kv, {"a"}) == ":-1\r\n");
        REQUIRE(handleDCR(kv, {"a"}) == ":-2\r\n");
        REQUIRE(handleDCR(kv, {"b"}) == ":4\r\n");
        REQUIRE(handleDCR(kv, {"c"}) == "-ERR value is not number or out of range\r\n");
    }

    SECTION("DCR bad args")
    {
        REQUIRE(handleDCR(kv, {"c", "d"}) == argumentError("1", 2));
    }
}

TEST_CASE("MGET method", "[mget][kvstore method][unit]")
{
    KVStore kv(false);
    kv.set("a", "1");
    kv.set("b", "2");

    SECTION("Mget expected")
    {
        auto res = kv.mget({"a", "b", "c"});
        REQUIRE(res.size() == 3);
        REQUIRE(res[0] == "1");
        REQUIRE(res[1] == "2");
        REQUIRE(res[2] == std::nullopt);
    }

    SECTION("Mget empty key list")
    {
        auto res = kv.mget({});
        REQUIRE(res.empty());
    }
}
TEST_CASE("MGET command", "[mget][command handler][unit]")
{
    KVStore kv(false);
    kv.set("a", "1");
    kv.set("b", "2");
    SECTION("MGET expected")
    {
        REQUIRE(handleMGET(kv, {"a", "b", "c"}) == "*3\r\n$1\r\n1\r\n$1\r\n2\r\n$-1\r\n");
    }

    SECTION("MGET bad args")
    {
        REQUIRE(handleMGET(kv, {}) == argumentError("1 or more", 0));
    }
}
//TODO APPEND, INCRBY DCRBY





