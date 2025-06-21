#define CATCH_CONFIG_MAIN
#include <catch2/catch_all.hpp>
#include <parser.hpp>
#include <kvstore.hpp>
#include <commands.hpp>
#include <util.hpp>

TEST_CASE("Set method", "[set][kvstore method][unit]")
{
    KVStore kv;
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
TEST_CASE("Get method", "[get][kvstore method][unit]")
{
    KVStore kv;
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
TEST_CASE("Del method", "[del][kvstore method][unit]")
{
    KVStore kv;
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
TEST_CASE("Exists method", "[exists][kvstore method][unit]")
{
    KVStore kv;
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
TEST_CASE("Incr method", "[incr][kvstore method][unit]")
{
    KVStore kv;
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
TEST_CASE("Dcr method", "[dcr][kvstore method][unit]")
{
    KVStore kv;
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

TEST_CASE("Ping command", "[ping][command handler][unit]")
{
    SECTION("Ping no arg")
    {
        REQUIRE(handlePing({}) == "+PONG\r\n");
    }

    SECTION("Ping message")
    {
        REQUIRE(handlePing({"hello world"}) == "$11\r\nhello world\r\n");
    }

    SECTION("Ping bad args")
    {
        REQUIRE(handlePing({"hello", "world"}) == argumentError("1 or none", 2));
    }

}
TEST_CASE("Echo command", "[echo][command handler][unit]")
{
    SECTION("Echo message")
    {
        REQUIRE(handleEcho({"sendback"}) == "$8\r\nsendback\r\n");
    }

    SECTION("Echo bad args")
    {
        REQUIRE(handleEcho({"send", "back"}) == argumentError("1", 2));
    }
}
TEST_CASE("Set command", "[set][command handler][unit]")
{
    KVStore kv;
    SECTION("Set expected")
    {
        REQUIRE(handleSet(kv, {"key","value"}) == "+OK\r\n");
        REQUIRE(kv.get("key") == "value");
    }
    SECTION("Set bad args")
    {
        REQUIRE(handleSet(kv, {"key2", "value2", "store"}) == argumentError("2", 3));
        REQUIRE(kv.get("key2") == std::nullopt);
    }
}
TEST_CASE("Get command", "[get][command handler][unit]")
{
    KVStore kv;
    SECTION("Get expected")
    {
        kv.set("a", "value");
        REQUIRE(handleGet(kv, {"a"}) == "$5\r\nvalue\r\n");
    }

    SECTION("Get non-existing")
    {
        REQUIRE(handleGet(kv, {"f"}) == "$-1\r\n");
    }

    SECTION("Get bad args")
    {
        REQUIRE(handleGet(kv, {"b", "c"}) == argumentError("1", 2));
    }
}
TEST_CASE("Del command", "[del][command handler][unit]")
{
    KVStore kv;
    kv.set("a", "value1");
    kv.set("b", "value2");
    kv.set("c", "value3");

    SECTION("Del expected")
    {
        REQUIRE(handleDel(kv,{"a", "e", "d"}) == ":1\r\n");
        REQUIRE(handleDel(kv,{"a", "b", "c"}) == ":2\r\n");
    }

    SECTION("Del bad args")
    {
        REQUIRE(handleDel(kv,{}) == argumentError("1 or more", 0));
    }
}
TEST_CASE("Exists command", "[exists][command handler][unit]")
{
    KVStore kv;
    kv.set("a", "1");
    kv.set("b", "2");
    kv.set("c", "3");
    SECTION("Exists expect")
    {
        REQUIRE(handleExists(kv, {"a", "d", "c"}) == ":2\r\n");
        REQUIRE(handleExists(kv, {"a", "b", "c"}) == ":3\r\n");
    }

    SECTION("Exists bad args")
    {
        REQUIRE(handleExists(kv, {}) == argumentError("1 or more", 0));
    }
}
TEST_CASE("Incr command", "[incr][command handler][unit]")
{
    KVStore kv;
    kv.set("b", "5");
    kv.set("c", "nonnum");
    SECTION("Incr expected")
    {
        REQUIRE(handleIncr(kv, {"a"}) == ":1\r\n");
        REQUIRE(handleIncr(kv, {"a"}) == ":2\r\n");
        REQUIRE(handleIncr(kv, {"b"}) == ":6\r\n");
        REQUIRE(handleIncr(kv, {"c"}) == "-ERR value is not number or out of range\r\n");
    }

    SECTION("Incr bad args")
    {
        REQUIRE(handleIncr(kv, {"c", "d"}) == argumentError("1", 2));
    }
}
TEST_CASE("Dcr command", "[dcr][command handler][unit]")
{
    KVStore kv;
    kv.set("b", "5");
    kv.set("c", "nonnum");
    SECTION("Dcr expected")
    {
        REQUIRE(handleDcr(kv, {"a"}) == ":-1\r\n");
        REQUIRE(handleDcr(kv, {"a"}) == ":-2\r\n");
        REQUIRE(handleDcr(kv, {"b"}) == ":4\r\n");
        REQUIRE(handleDcr(kv, {"c"}) == "-ERR value is not number or out of range\r\n");
    }

    SECTION("Dcr bad args")
    {
        REQUIRE(handleDcr(kv, {"c", "d"}) == argumentError("1", 2));
    }
}
TEST_CASE("Expire command", "[expire][command handler][unit]")
{
    KVStore kv;
    kv.set("a", "1");
    SECTION("Expire expected")
    {
        REQUIRE(handleExpire(kv, {"a", "30"}) == ":1\r\n");
        REQUIRE(handleExpire(kv, {"b", "60"}) == ":0\r\n");
    }

    SECTION("Expire bad args")
    {
        REQUIRE(handleExpire(kv, {"a"}) == argumentError("2", 1));
        REQUIRE(handleExpire(kv, {"a", "50", "val"}) == argumentError("2", 3));
    }
}
TEST_CASE("TTL command", "[ttl][command handler][unit]")
{
    KVStore kv;
    kv.set("a", "1");
    SECTION("TLL expected")
    {
        REQUIRE(handleTTL(kv, {"a"}) == ":-1\r\n");
        handleExpire(kv, {"a", "10"});
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
TEST_CASE("Flushall command", "[flushall][command handler][unit]")
{
    KVStore kv;
    SECTION("Flushall expected")
    {
        REQUIRE(handleFlushall(kv, {0}) == "\r\n\r\n");
    }

    SECTION("Flushall bad args")
    {
        REQUIRE(handleFlushall(kv, {"a"}) == argumentError("0", 1));
    }
}
TEST_CASE("Mget command", "[mget][command handler][unit]")
{
    KVStore kv;
    kv.set("a", "1");
    kv.set("b", "2");
    SECTION("Mget expected")
    {
        REQUIRE(handleMget(kv, {"a", "b", "c"}) == "*3\r\n$1\r\n1\r\n$1\r\n2\r\n$-1\r\n");
    }

    SECTION("Mget bad args")
    {
        REQUIRE(handleMget(kv, {0}) == argumentError("1 or more", 0));
    }
}