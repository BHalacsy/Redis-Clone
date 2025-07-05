#define CATCH_CONFIG_MAIN
#include <catch2/catch_all.hpp>
#include <parser.hpp>
#include <kvstore.hpp>
#include <commands.hpp>
#include <thread>
#include <util.hpp>

TEST_CASE("Set method", "[set][kvstore method][unit]")
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
TEST_CASE("Get method", "[get][kvstore method][unit]")
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
TEST_CASE("Del method", "[del][kvstore method][unit]")
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
TEST_CASE("Exists method", "[exists][kvstore method][unit]")
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
TEST_CASE("Incr method", "[incr][kvstore method][unit]")
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
TEST_CASE("Dcr method", "[dcr][kvstore method][unit]")
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
TEST_CASE("Expire method", "[expire][kvstore method][unit]")
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
TEST_CASE("Ttl method", "[ttl][kvstore method][unit]")
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
TEST_CASE("Mget method", "[mget][kvstore method][unit]")
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
