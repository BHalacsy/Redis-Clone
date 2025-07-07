#define CATCH_CONFIG_MAIN

#include <thread>
#include <catch2/catch_test_macros.hpp>

#include "parser.hpp"
#include "kvstore.hpp"
#include "commands.hpp"
#include "util.hpp"


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
    {
        KVStore kv(true, filename);
        REQUIRE(kv.get("a") == "1");
        REQUIRE(kv.lpop("b") == "1");
        REQUIRE(kv.hget("c", "d") == "1");
    }
}