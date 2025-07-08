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

TEST_CASE("INCRBY method", "[incrby][kvstore method][unit]")
{
    KVStore kv(false);

    SECTION("Incrby on new key")
    {
        REQUIRE(kv.incrby("a", 5) == 5);
        REQUIRE(kv.get("a") == "5");
    }

    SECTION("Incrby on existing key")
    {
        kv.set("a", "2");
        REQUIRE(kv.incrby("a", 3) == 5);
        REQUIRE(kv.get("a") == "5");
    }

    SECTION("Incrby on non-number")
    {
        kv.set("b", "string");
        REQUIRE(kv.incrby("b", 2) == std::nullopt);
    }

    SECTION("Incrby negative value")
    {
        kv.set("c", "10");
        REQUIRE(kv.incrby("c", -4) == 6);
        REQUIRE(kv.get("c") == "6");
    }
}
TEST_CASE("INCRBY command", "[incrby][command handler][unit]")
{
    KVStore kv(false);

    SECTION("INCRBY expected")
    {
        REQUIRE(handleINCRBY(kv, {"a", "7"}) == ":7\r\n");
        REQUIRE(handleINCRBY(kv, {"a", "3"}) == ":10\r\n");
    }

    SECTION("INCRBY non-number value")
    {
        kv.set("b", "foo");
        REQUIRE(handleINCRBY(kv, {"b", "2"}) == "-ERR value is not number or out of range\r\n");
    }

    SECTION("INCRBY bad args")
    {
        REQUIRE(handleINCRBY(kv, {"a"}) == argumentError("2", 1));
        REQUIRE(handleINCRBY(kv, {"a", "1", "2"}) == argumentError("2", 3));
    }

    SECTION("INCRBY non-integer increment")
    {
        REQUIRE(handleINCRBY(kv, {"a", "string"}) == "-ERR arg given not a number\r\n");
    }
}

TEST_CASE("DCRBY method", "[dcrby][kvstore method][unit]")
{
    KVStore kv(false);

    SECTION("Dcrby on new key")
    {
        REQUIRE(kv.dcrby("a", 4) == -4);
        REQUIRE(kv.get("a") == "-4");
    }

    SECTION("Dcrby on existing key")
    {
        kv.set("a", "10");
        REQUIRE(kv.dcrby("a", 3) == 7);
        REQUIRE(kv.get("a") == "7");
    }

    SECTION("Dcrby on non-number")
    {
        kv.set("b", "string");
        REQUIRE(kv.dcrby("b", 2) == std::nullopt);
    }

    SECTION("Dcrby negative value")
    {
        kv.set("c", "5");
        REQUIRE(kv.dcrby("c", -2) == 7);
        REQUIRE(kv.get("c") == "7");
    }
}
TEST_CASE("DCRBY command", "[dcrby][command handler][unit]")
{
    KVStore kv(false);

    SECTION("DCRBY expected")
    {
        REQUIRE(handleDCRBY(kv, {"a", "2"}) == ":-2\r\n");
        REQUIRE(handleDCRBY(kv, {"a", "3"}) == ":-5\r\n");
    }

    SECTION("DCRBY non-number value")
    {
        kv.set("b", "foo");
        REQUIRE(handleDCRBY(kv, {"b", "2"}) == "-ERR value is not number or out of range\r\n");
    }

    SECTION("DCRBY bad args")
    {
        REQUIRE(handleDCRBY(kv, {"a"}) == argumentError("2", 1));
        REQUIRE(handleDCRBY(kv, {"a", "1", "2"}) == argumentError("2", 3));
    }

    SECTION("DCRBY non-integer decrement")
    {
        REQUIRE(handleDCRBY(kv, {"a", "string"}) == "-ERR arg given not a number\r\n");
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

TEST_CASE("APPEND method", "[append][kvstore method][unit]")
{
    KVStore kv(false);

    SECTION("Append to new key")
    {
        REQUIRE(kv.append("a", "word") == 4);
        REQUIRE(kv.get("a") == "word");
    }

    SECTION("Append to existing key")
    {
        kv.set("a", "fizz");
        REQUIRE(kv.append("a", "buzz") == 8);
        REQUIRE(kv.get("a") == "fizzbuzz");
    }
}
TEST_CASE("APPEND command", "[append][command handler][unit]")
{
    KVStore kv(false);

    SECTION("APPEND expected")
    {
        REQUIRE(handleAPPEND(kv, {"a", "foo"}) == ":3\r\n");
        REQUIRE(handleAPPEND(kv, {"a", "bar"}) == ":6\r\n");
        REQUIRE(kv.get("a") == "foobar");
    }

    SECTION("APPEND bad args")
    {
        REQUIRE(handleAPPEND(kv, {"a"}) == argumentError("2", 1));
        REQUIRE(handleAPPEND(kv, {}) == argumentError("2", 0));
    }
}





