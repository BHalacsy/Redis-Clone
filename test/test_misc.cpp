#define CATCH_CONFIG_MAIN

#include <thread>
#include <catch2/catch_test_macros.hpp>

#include "parser.hpp"
#include "kvstore.hpp"
#include "commands.hpp"
#include "util.hpp"

TEST_CASE("TYPE command", "[type][command handler][unit]")
{

    KVStore kv(false);
    kv.set("string", "sval");
    kv.rpush({"list", "lval"});
    kv.sadd({"set", "setval"});
    kv.hset({"hash", "f", "hval"});

    SECTION("TYPE expected")
    {
        REQUIRE(handleTYPE(kv, {"string"}) == "+string\r\n");
        REQUIRE(handleTYPE(kv, {"list"}) == "+list\r\n");
        REQUIRE(handleTYPE(kv, {"set"}) == "+set\r\n");
        REQUIRE(handleTYPE(kv, {"hash"}) == "+hash\r\n");
    }

    SECTION("TYPE non-existing key")
    {
        REQUIRE(handleTYPE(kv, {"non-key"}) == "+none\r\n");
    }

    SECTION("TYPE bad args")
    {
        REQUIRE(handleTYPE(kv, {}) == argumentError("1", 0));
        REQUIRE(handleTYPE(kv, {"string", "extra"}) == argumentError("1", 2));
    }
}