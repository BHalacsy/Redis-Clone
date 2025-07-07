#define CATCH_CONFIG_MAIN

#include <thread>
#include <catch2/catch_test_macros.hpp>

#include "parser.hpp"
#include "kvstore.hpp"
#include "commands.hpp"
#include "util.hpp"


TEST_CASE("DEL method", "[del][kvstore method][unit]")
{
    KVStore kv(false);
    kv.set("a", "1");
    kv.set("b", "2");

    SECTION("Delete existing key")
    {
        REQUIRE(kv.del({"a"}) == 1);
        REQUIRE(kv.exists({"a"}) == 0);
        REQUIRE(kv.get("a") == std::nullopt);
    }

    SECTION("Delete non-existing key")
    {
        REQUIRE(kv.del({"x"}) == 0);
    }

    SECTION("Delete multiple keys")
    {
        REQUIRE(kv.del({"x", "a", "b"}) == 2);
    }
}
TEST_CASE("EXISTS method", "[exists][kvstore method][unit]")
{
    KVStore kv(false);
    kv.set("a", "1");
    kv.set("b", "2");

    SECTION("Check existing keys")
    {
        REQUIRE(kv.exists({"a"}) == 1);
    }

    SECTION("Check non-existing keys")
    {
        REQUIRE(kv.exists({"y"}) == 0);
    }

    SECTION("Check mix of existing and non-existing keys")
    {
        REQUIRE(kv.exists({"a", "y", "b"}) == 2);
    }
}
TEST_CASE("DEL command", "[del][command handler][unit]")
{
    KVStore kv(false);
    kv.set("a", "value1");
    kv.set("b", "value2");
    kv.set("c", "value3");

    SECTION("DEL expected")
    {
        REQUIRE(handleDEL(kv,{"a", "e", "d"}) == ":1\r\n");
        REQUIRE(handleDEL(kv,{"a", "b", "c"}) == ":2\r\n");
    }

    SECTION("DEL bad args")
    {
        REQUIRE(handleDEL(kv,{}) == argumentError("1 or more", 0));
    }
}
TEST_CASE("EXISTS command", "[exists][command handler][unit]")
{
    KVStore kv(false);
    kv.set("a", "1");
    kv.set("b", "2");
    kv.set("c", "3");
    SECTION("EXISTS expect")
    {
        REQUIRE(handleEXISTS(kv, {"a", "d", "c"}) == ":2\r\n");
        REQUIRE(handleEXISTS(kv, {"a", "b", "c"}) == ":3\r\n");
    }

    SECTION("EXISTS bad args")
    {
        REQUIRE(handleEXISTS(kv, {}) == argumentError("1 or more", 0));
    }
}
//TODO FLUSHALL method?

TEST_CASE("PING command", "[ping][command handler][unit]")
{
    SECTION("PING no arg")
    {
        REQUIRE(handlePING({}) == "+PONG\r\n");
    }

    SECTION("PING message")
    {
        REQUIRE(handlePING({"hello world"}) == "$11\r\nhello world\r\n");
    }

    SECTION("PING bad args")
    {
        REQUIRE(handlePING({"hello", "world"}) == argumentError("1 or none", 2));
    }

}
TEST_CASE("ECHO command", "[echo][command handler][unit]")
{
    SECTION("ECHO message")
    {
        REQUIRE(handleECHO({"sendback"}) == "$8\r\nsendback\r\n");
    }

    SECTION("ECHO bad args")
    {
        REQUIRE(handleECHO({"send", "back"}) == argumentError("1", 2));
    }
}
TEST_CASE("FLUSHALL command", "[flushall][command handler][unit]")
{
    KVStore kv(false);
    kv.set("a", "1");
    kv.set("b", "2");
    kv.set("c", "3");
    SECTION("FLUSHALL expected")
    {
        REQUIRE(handleFLUSHALL(kv, {}) == "+OK\r\n");
        REQUIRE(handleGET(kv, {"a"}) == "$-1\r\n");
    }

    SECTION("FLUSHALL bad args")
    {
        REQUIRE(handleFLUSHALL(kv, {"a"}) == argumentError("0", 1));
    }
}

