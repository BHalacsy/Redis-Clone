#define CATCH_CONFIG_MAIN

#include <set>
#include <thread>
#include <catch2/catch_test_macros.hpp>

#include "parser.hpp"
#include "kvstore.hpp"
#include "commands.hpp"
#include "util.hpp"


TEST_CASE("SADD method", "[sadd][kvstore method][unit]")
{
    KVStore kv(false);

    SECTION("Sadd expected and add existing")
    {
        REQUIRE(kv.sadd({"myset", "a", "b", "c"}) == 3);
        REQUIRE(kv.sadd({"myset", "b", "d"}) == 1);
        REQUIRE(kv.sadd({"myset", "b", "d"}) == 0);
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

TEST_CASE("SREM method", "[srem][kvstore method][unit]")
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

TEST_CASE("SISMEMBER method", "[sismember][kvstore method][unit]")
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

TEST_CASE("SMEMBERS method", "[Smembers][kvstore method][unit]")
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

TEST_CASE("SCARD method", "[Scard][kvstore method][unit]")
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

TEST_CASE("SPOP method", "[spop][kvstore method][unit]")
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