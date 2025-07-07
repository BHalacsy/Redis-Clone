#define CATCH_CONFIG_MAIN

#include <thread>
#include <catch2/catch_test_macros.hpp>

#include "parser.hpp"
#include "kvstore.hpp"
#include "commands.hpp"
#include "util.hpp"


TEST_CASE("LPUSH and RPUSH methods", "[lpush/rpush][kvstore method][unit]")
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

TEST_CASE("LPOP and RPOP methods", "[lpop/rpop][kvstore method][unit]")
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

TEST_CASE("LRANGE method", "[lrange][kvstore method][unit]")
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

TEST_CASE("LLEN method", "[llen][kvstore method][unit]")
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

TEST_CASE("LINDEX method", "[lindex][kvstore method][unit]")
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

TEST_CASE("LSET method", "[lset][kvstore method][unit]")
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

TEST_CASE("LREM method", "[lrem][kvstore method][unit]")
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
