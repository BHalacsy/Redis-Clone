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
TEST_CASE("Dcr method", "[Dcr][kvstore method][unit]")
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