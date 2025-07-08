#define CATCH_CONFIG_MAIN

#include <thread>
#include <catch2/catch_test_macros.hpp>

#include "parser.hpp"
#include "kvstore.hpp"
#include "commands.hpp"
#include "util.hpp"


TEST_CASE("EXPIRE method", "[expire][kvstore method][unit]")
{
    KVStore kv(false);
    kv.set("a", "1");
    kv.set("b", "2");

    SECTION("Set expiry on existing key")
    {
        REQUIRE(kv.expire("a", 5) == 1);
        REQUIRE(kv.ttl("a") <= 5);
        REQUIRE(kv.ttl("a") > 0);
    }

    SECTION("Set expiry on non-existing key")
    {
        REQUIRE(kv.expire("c", 5) == 0);
    }

    SECTION("Key expires after time")
    {
        kv.expire("b", 1);
        std::this_thread::sleep_for(std::chrono::seconds(1));
        REQUIRE(kv.get("b") == std::nullopt);
        REQUIRE(kv.ttl("b") == -2);
    }

}
TEST_CASE("EXPIRE command", "[expire][command handler][unit]")
{
    KVStore kv(false);
    kv.set("a", "1");
    SECTION("EXPIRE expected")
    {
        REQUIRE(handleEXPIRE(kv, {"a", "30"}) == ":1\r\n");
        REQUIRE(handleEXPIRE(kv, {"b", "60"}) == ":0\r\n");
    }

    SECTION("EXPIRE bad args")
    {
        REQUIRE(handleEXPIRE(kv, {"a"}) == argumentError("2", 1));
        REQUIRE(handleEXPIRE(kv, {"a", "50", "val"}) == argumentError("2", 3));
    }
}

TEST_CASE("TTL method", "[ttl][kvstore method][unit]")
{
    KVStore kv(false);
    kv.set("a", "1");

    SECTION("TTL for key without expiry")
    {
        REQUIRE(kv.ttl("a") == -1);
    }

    SECTION("TTL for non-existing key")
    {
        REQUIRE(kv.ttl("b") == -2);
    }

    SECTION("TTL decreases over time")
    {
        kv.expire("a", 5);
        int ttl1 = kv.ttl("a");
        std::this_thread::sleep_for(std::chrono::seconds(1));
        int ttl2 = kv.ttl("a");
        REQUIRE(ttl2 < ttl1);
        REQUIRE(ttl2 > 0);
    }
}
TEST_CASE("TTL command", "[ttl][command handler][unit]")
{
    KVStore kv(false);
    kv.set("a", "1");
    SECTION("TLL expected")
    {
        REQUIRE(handleTTL(kv, {"a"}) == ":-1\r\n");
        handleEXPIRE(kv, {"a", "10"});
        std::string ttl = handleTTL(kv, {"a"});
        int ttlNum = intParser(ttl);
        REQUIRE(ttlNum < 11);
        REQUIRE(2 < ttlNum);

        REQUIRE(handleTTL(kv, {"b"}) == ":-2\r\n");
    }

    SECTION("TLL bad args")
    {
        REQUIRE(handleTTL(kv, {"a", "b"}) == argumentError("1", 2));
    }
}

TEST_CASE("PERSIST method", "[persist][kvstore method][unit]")
{
    KVStore kv(false);
    kv.set("a", "1");

    SECTION("Persist on key with expiry")
    {
        kv.expire("a", 10);
        REQUIRE(kv.ttl("a") > 0);
        REQUIRE(kv.persist("a") == true);
        REQUIRE(kv.ttl("a") == -1);
    }

    SECTION("Persist on non-existing key")
    {
        REQUIRE(kv.persist("b") == false);
    }
}
TEST_CASE("PERSIST command", "[persist][command handler][unit]")
{
    KVStore kv(false);
    kv.set("a", "1");

    SECTION("PERSIST expected")
    {
        kv.expire("a", 10);
        REQUIRE(handlePERSIST(kv, {"a"}) == ":1\r\n");
        REQUIRE(kv.ttl("a") == -1);
        REQUIRE(handlePERSIST(kv, {"b"}) == ":0\r\n");
    }

    SECTION("PERSIST bad args")
    {
        REQUIRE(handlePERSIST(kv, {}) == argumentError("1", 0));
        REQUIRE(handlePERSIST(kv, {"a", "b"}) == argumentError("1", 2));
    }
}