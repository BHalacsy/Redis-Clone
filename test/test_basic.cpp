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
        std::this_thread::sleep_for(std::chrono::seconds(2));
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
        std::this_thread::sleep_for(std::chrono::seconds(2));
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
// TEST_CASE("Lpush and Rpush methods", "[lpush/rpush][kvstore method][unit]")
// {
//     KVStore kv(false);
//
//     SECTION("Lpush and Rpush expected")
//     {
//         REQUIRE(kv.lpush({"mylist", "a"}) == 1);
//         REQUIRE(kv.lpush({"mylist", "b"}) == 2);
//         REQUIRE(kv.rpush({"mylist", "c"}) == 3);
//         REQUIRE(kv.rpush({"mylist", "d"}) == 4);
//
//         auto range = kv.lrange("mylist", 0, -1);
//         REQUIRE(range.size() == 4);
//         REQUIRE(range[0] == "b");
//         REQUIRE(range[1] == "a");
//         REQUIRE(range[2] == "c");
//         REQUIRE(range[3] == "d");
//     }
// }
// TEST_CASE("Lpop and Rpop methods", "[lpop/rpop][kvstore method][unit]")
// {
//     KVStore kv(false);
//
//     SECTION("Lpop and Rpop expected")
//     {
//         kv.rpush({"mylist", "a"});
//         kv.rpush({"mylist", "b"});
//         kv.rpush({"mylist", "c"});
//         REQUIRE(kv.lpop("mylist").value() == "a");
//         REQUIRE(kv.rpop("mylist").value() == "c");
//         REQUIRE(kv.lpop("mylist").value() == "b");
//     }
//
//     SECTION("Pop empty list")
//     {
//         REQUIRE(kv.lpop("mylist") == std::nullopt);
//     }
// }
// TEST_CASE("Lrange method", "[lrange][kvstore method][unit]")
// {
//     KVStore kv(false);
//
//     SECTION("lrange expected")
//     {
//         kv.rpush({"mylist", "a"});
//         kv.rpush({"mylist", "b"});
//         kv.rpush({"mylist", "c"});
//         REQUIRE(kv.llen("mylist") == 3);
//         kv.lpop("mylist");
//         REQUIRE(kv.llen("mylist") == 2);
//
//     }
//
//     SECTION("lrange empty list")
//     {
//         kv.lpop("mylist");
//         kv.lpop("mylist");
//         REQUIRE(kv.llen("mylist") == 0);
//     }
// }
// TEST_CASE("Llen method", "[llen][kvstore method][unit]")
// {
//     KVStore kv(false);
//
//     SECTION("Llen expected")
//     {
//         kv.rpush({"mylist", "a"});
//         kv.rpush({"mylist", "b"});
//         kv.rpush({"mylist", "c"});
//         REQUIRE(kv.llen("mylist") == 3);
//         kv.lpop("mylist");
//         REQUIRE(kv.llen("mylist") == 2);
//
//     }
//
//     SECTION("Llen empty list")
//     {
//         kv.lpop("mylist");
//         kv.lpop("mylist");
//         REQUIRE(kv.llen("mylist") == 0);
//     }
// }
// TEST_CASE("Lindex method", "[lindex][kvstore method][unit]")
// {
//     KVStore kv(false);
//
//     SECTION("Lindex expected")
//     {
//         kv.rpush({"mylist", "a"});
//         kv.rpush({"mylist", "b"});
//         kv.rpush({"mylist", "c"});
//         REQUIRE(kv.lindex("mylist", 0) == "a");
//         REQUIRE(kv.lindex("mylist", 2) == "c");
//     }
//
//     SECTION("Lindex non value empty in list")
//     {
//         REQUIRE(kv.lindex("mylist", 3) == std::nullopt);
//     }
// }
// TEST_CASE("Lset method", "[lset][kvstore method][unit]")
// {
//     KVStore kv(false);
//
//     SECTION("Lset expected")
//     {
//         kv.rpush({"mylist", "a"});
//         kv.rpush({"mylist", "b"});
//         kv.rpush({"mylist", "c"});
//         REQUIRE(kv.lset("mylist", 1, "z"));
//         REQUIRE(kv.lindex("mylist", 1) == "z");
//         REQUIRE(kv.lset("mylist", 2, "x"));
//         REQUIRE(kv.lindex("mylist", 2) == "x");
//     }
//
//     SECTION("Lset non value in list")
//     {
//         REQUIRE(!kv.lset("mylist", 5, "x"));
//     }
// }
// TEST_CASE("Lrem method", "[lrem][kvstore method][unit]")
// {
//     KVStore kv(false);
//
//     SECTION("Lrem expected")
//     {
//         kv.rpush({"mylist", "a"});
//         kv.rpush({"mylist", "a"});
//         kv.rpush({"mylist", "a"});
//         kv.rpush({"mylist", "b"});
//         kv.rpush({"mylist", "b"});
//         REQUIRE(kv.lrem("mylist", 1, "a") == 1);
//         REQUIRE(kv.lrem("mylist", 0, "a") == 2);
//         REQUIRE(kv.lindex("mylist", 0) == "b");
//     }
//
//     SECTION("Lrem non value in list")
//     {
//         REQUIRE(kv.lrem("mylist", 0, "c") == 0);
//         REQUIRE(kv.lrem("otherlist", 0, "c") == 0);
//     }
// }
// TEST_CASE("Sadd method", "[sadd][kvstore method][unit]")
// {
//     KVStore kv(false);
//
//     SECTION("Sadd expected")
//     {
//         REQUIRE(kv.sadd({"myset", "a", "b", "c"}) == 3);
//     }
//
//     SECTION("Sadd existing")
//     {
//         REQUIRE(kv.sadd({"myset", "b", "d"}) == 1);
//     }
// }
// TEST_CASE("Srem method", "[srem][kvstore method][unit]")
// {
//     KVStore kv(false);
//     kv.sadd({"myset", "a", "b", "c"});
//
//     SECTION("Srem expected")
//     {
//         REQUIRE(kv.srem({"myset", "b", "x"}) == 1);
//         REQUIRE(kv.sismember("myset", "b") == false);
//     }
//
//     SECTION("Srem non-existing set")
//     {
//         REQUIRE(kv.srem({"otherset", "a", "c"}) == 0);
//     }
// }
// TEST_CASE("Sismember method", "[sismember][kvstore method][unit]")
// {
//     KVStore kv(false);
//     kv.sadd({"myset", "a", "b", "c"});
//
//     SECTION("Sismember expected")
//     {
//         REQUIRE(kv.sismember("myset", "b") == true);
//         REQUIRE(kv.sismember("myset", "x") == false);
//
//     }
//
//     SECTION("Sismember non-existing set")
//     {
//         REQUIRE(kv.sismember("otherset", "a") == false);
//     }
// }
// TEST_CASE("Smembers method", "[Smembers][kvstore method][unit]")
// {
//     KVStore kv(false);
//     kv.sadd({"myset", "a", "b", "c"});
//
//     SECTION("Smembers expected")
//     {
//         auto members = kv.smembers("myset");
//         std::set<std::string> res;
//         for (auto i : members) if (i) res.insert(*i);
//         REQUIRE(res == std::set<std::string>{"a", "b", "c"});
//     }
//
//     SECTION("Smembers non-existing set")
//     {
//         REQUIRE(kv.smembers("otherset").empty());
//     }
// }
// TEST_CASE("Scard method", "[Scard][kvstore method][unit]")
// {
//     KVStore kv(false);
//     kv.sadd({"myset", "a", "b", "c"});
//
//     SECTION("Scard expected")
//     {
//         REQUIRE(kv.scard("myset") == 3);
//         kv.srem({"myset", "a"});
//         REQUIRE(kv.scard("myset") == 2);
//     }
//
//     SECTION("Scard non-existing set")
//     {
//         REQUIRE(kv.scard("otherset") == 0);
//     }
// }
// TEST_CASE("Spops method", "[spop][kvstore method][unit]")
// {
//     KVStore kv(false);
//     kv.sadd({"myset", "a", "b", "c"});
//
//     SECTION("Spop expected")
//     {
//         auto popped = kv.spop("myset", 2);
//         REQUIRE(popped.size() == 2);
//         REQUIRE(kv.scard("myset") == 1);
//     }
//
//     SECTION("Spop non-existing set")
//     {
//         auto popped = kv.spop("otherset", 2);
//         REQUIRE(popped.empty());
//     }
// }
// TEST_CASE("Hset and Hget methods", "[hset/hget][kvstore method][unit]")
// {
//     KVStore kv(false);
//
//     SECTION("Hset and Hget new elements expected")
//     {
//         REQUIRE(kv.hset("myhash", "f1", "v1") == 1);
//         REQUIRE(kv.hget("myhash", "f1") == "v1");
//     }
//
//     SECTION("Hset overwrite field") {
//         REQUIRE(kv.hset("myhash", "f1", "v2") == 0);
//         REQUIRE(kv.hget("myhash", "f1") == "v2");
//     }
//     SECTION("Hget empty key and empty field list")
//     {
//         REQUIRE(kv.hget("otherhash", "f1") == std::nullopt);
//         REQUIRE(kv.hget("myhash", "otherfield") == std::nullopt);
//     }
// }
// TEST_CASE("Hdel and Hexists methods", "[hdel/hexists][kvstore method][unit]")
// {
//     KVStore kv(false);
//     kv.hmset({"myhash", "f1", "v1", "f2", "v2"});
//
//     SECTION("Hdel and Hexists new elements expected")
//     {
//         REQUIRE(kv.hdel({"myhash", "f1"}) == 1);
//         REQUIRE(kv.hexists({"myhash", "f1"}) == false);
//         REQUIRE(kv.hexists({"myhash", "f2"}) == true);
//     }
//
//     SECTION("Hdel non-existing field") {
//         REQUIRE(kv.hdel({"myhash", "f3"}) == 0);
//     }
// }
// TEST_CASE("Hlen, Hkeys, Hvals methods", "[hlen/hkeys/hvals][kvstore method][unit]")
// {
//     KVStore kv(false);
//     kv.hmset({"myhash", "f1", "v1", "f2", "v2", "f3", "v3"});
//
//     SECTION("Hlen expected")
//     {
//         REQUIRE(kv.hlen("myhash") == 3);
//     }
//
//     SECTION("Hkeys returns all fields") {
//         auto keys = kv.hkeys("myhash");
//         std::set<std::string> expected = {"f1", "f2", "f3"};
//         std::set<std::string> actual(keys.begin(), keys.end());
//         REQUIRE(actual == expected);
//     }
//
//     SECTION("Hvals returns all values") {
//         auto vals = kv.hvals("myhash");
//         std::set<std::string> expected = {"v1", "v2", "v3"};
//         std::set<std::string> actual(vals.begin(), vals.end());
//         REQUIRE(actual == expected);
//     }
// }
// TEST_CASE("Hmset and Hmget methods", "[hmset/hmget][kvstore method][unit]")
// {
//     KVStore kv(false);
//     SECTION("Hmset and Hmget expected")
//     {
//         REQUIRE(kv.hmset({"myhash", "f1", "v1", "f2", "v2", "f3", "v3"}) == true);
//         auto vals = kv.hmget({"myhash", "f1", "f2", "f3", "f4"});
//         REQUIRE(vals.size() == 4);
//         REQUIRE(vals[0] == "v1");
//         REQUIRE(vals[1] == "v2");
//         REQUIRE(vals[2] == "v3");
//         REQUIRE(vals[3] == std::nullopt);
//     }
//
//     SECTION("Hmset with empty field-value list")
//     {
//         REQUIRE(kv.hmset({"myhash"}) == false);
//     }
//
//     SECTION("Hmget on non-existing hash")
//     {
//         auto vals = kv.hmget({"nohash", "f1", "f2"});
//         REQUIRE(vals.size() == 2);
//         REQUIRE(vals[0] == std::nullopt);
//         REQUIRE(vals[1] == std::nullopt);
//     }
// }


TEST_CASE("Ping command", "[ping][command handler][unit]")
{
    SECTION("Ping no arg")
    {
        REQUIRE(handlePING({}) == "+PONG\r\n");
    }

    SECTION("Ping message")
    {
        REQUIRE(handlePING({"hello world"}) == "$11\r\nhello world\r\n");
    }

    SECTION("Ping bad args")
    {
        REQUIRE(handlePING({"hello", "world"}) == argumentError("1 or none", 2));
    }

}
TEST_CASE("Echo command", "[echo][command handler][unit]")
{
    SECTION("Echo message")
    {
        REQUIRE(handleECHO({"sendback"}) == "$8\r\nsendback\r\n");
    }

    SECTION("Echo bad args")
    {
        REQUIRE(handleECHO({"send", "back"}) == argumentError("1", 2));
    }
}
TEST_CASE("Set command", "[set][command handler][unit]")
{
    KVStore kv(false);
    SECTION("Set expected")
    {
        REQUIRE(handleSET(kv, {"key","value"}) == "+OK\r\n");
        REQUIRE(kv.get("key") == "value");
    }
    SECTION("Set bad args")
    {
        REQUIRE(handleSET(kv, {"key2", "value2", "store"}) == argumentError("2", 3));
        REQUIRE(kv.get("key2") == std::nullopt);
    }
}
TEST_CASE("Get command", "[get][command handler][unit]")
{
    KVStore kv(false);
    SECTION("Get expected")
    {
        kv.set("a", "value");
        REQUIRE(handleGET(kv, {"a"}) == "$5\r\nvalue\r\n");
    }

    SECTION("Get non-existing")
    {
        REQUIRE(handleGET(kv, {"f"}) == "$-1\r\n");
    }

    SECTION("Get bad args")
    {
        REQUIRE(handleGET(kv, {"b", "c"}) == argumentError("1", 2));
    }
}
TEST_CASE("Del command", "[del][command handler][unit]")
{
    KVStore kv(false);
    kv.set("a", "value1");
    kv.set("b", "value2");
    kv.set("c", "value3");

    SECTION("Del expected")
    {
        REQUIRE(handleDEL(kv,{"a", "e", "d"}) == ":1\r\n");
        REQUIRE(handleDEL(kv,{"a", "b", "c"}) == ":2\r\n");
    }

    SECTION("Del bad args")
    {
        REQUIRE(handleDEL(kv,{}) == argumentError("1 or more", 0));
    }
}
TEST_CASE("Exists command", "[exists][command handler][unit]")
{
    KVStore kv(false);
    kv.set("a", "1");
    kv.set("b", "2");
    kv.set("c", "3");
    SECTION("Exists expect")
    {
        REQUIRE(handleEXISTS(kv, {"a", "d", "c"}) == ":2\r\n");
        REQUIRE(handleEXISTS(kv, {"a", "b", "c"}) == ":3\r\n");
    }

    SECTION("Exists bad args")
    {
        REQUIRE(handleEXISTS(kv, {}) == argumentError("1 or more", 0));
    }
}
TEST_CASE("Incr command", "[incr][command handler][unit]")
{
    KVStore kv(false);
    kv.set("b", "5");
    kv.set("c", "nonnum");
    SECTION("Incr expected")
    {
        REQUIRE(handleINCR(kv, {"a"}) == ":1\r\n");
        REQUIRE(handleINCR(kv, {"a"}) == ":2\r\n");
        REQUIRE(handleINCR(kv, {"b"}) == ":6\r\n");
        REQUIRE(handleINCR(kv, {"c"}) == "-ERR value is not number or out of range\r\n");
    }

    SECTION("Incr bad args")
    {
        REQUIRE(handleINCR(kv, {"c", "d"}) == argumentError("1", 2));
    }
}
TEST_CASE("Dcr command", "[dcr][command handler][unit]")
{
    KVStore kv(false);
    kv.set("b", "5");
    kv.set("c", "nonnum");
    SECTION("Dcr expected")
    {
        REQUIRE(handleDCR(kv, {"a"}) == ":-1\r\n");
        REQUIRE(handleDCR(kv, {"a"}) == ":-2\r\n");
        REQUIRE(handleDCR(kv, {"b"}) == ":4\r\n");
        REQUIRE(handleDCR(kv, {"c"}) == "-ERR value is not number or out of range\r\n");
    }

    SECTION("Dcr bad args")
    {
        REQUIRE(handleDCR(kv, {"c", "d"}) == argumentError("1", 2));
    }
}
TEST_CASE("Expire command", "[expire][command handler][unit]")
{
    KVStore kv(false);
    kv.set("a", "1");
    SECTION("Expire expected")
    {
        REQUIRE(handleEXPIRE(kv, {"a", "30"}) == ":1\r\n");
        REQUIRE(handleEXPIRE(kv, {"b", "60"}) == ":0\r\n");
    }

    SECTION("Expire bad args")
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
TEST_CASE("Flushall command", "[flushall][command handler][unit]")
{
    KVStore kv(false);
    kv.set("a", "1");
    kv.set("b", "2");
    kv.set("c", "3");
    SECTION("Flushall expected")
    {
        REQUIRE(handleFLUSHALL(kv, {}) == "+OK\r\n");
        REQUIRE(handleGET(kv, {"a"}) == "$-1\r\n");
    }

    SECTION("Flushall bad args")
    {
        REQUIRE(handleFLUSHALL(kv, {"a"}) == argumentError("0", 1));
    }
}
TEST_CASE("Mget command", "[mget][command handler][unit]")
{
    KVStore kv(false);
    kv.set("a", "1");
    kv.set("b", "2");
    SECTION("Mget expected")
    {
        REQUIRE(handleMGET(kv, {"a", "b", "c"}) == "*3\r\n$1\r\n1\r\n$1\r\n2\r\n$-1\r\n");
    }

    SECTION("Mget bad args")
    {
        REQUIRE(handleMGET(kv, {}) == argumentError("1 or more", 0));
    }
}

// TEST_CASE(" method", "[][kvstore method][unit]")
// {
//     KVStore kv(false);
//
//     SECTION(" expected")
//     {
//         REQUIRE();
//     }
//
//     SECTION(" empty key list")
//     {
//         REQUIRE();
//     }
// }