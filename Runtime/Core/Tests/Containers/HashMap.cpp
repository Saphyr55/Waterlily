#include "Waterlily/Core/Containers/HashMap.hpp"
#include "Waterlily/Core/String/Format.hpp"
#include "Waterlily/Core/String/String.hpp"

#include <catch2/catch_all.hpp>

using namespace Wl;

TEST_CASE("Check insertion.", "[HashMap::put]")
{
    HashMap<uint32_t, uint32_t> map;

    map.Put(5, 8);
    REQUIRE(map.Get(5) == 8);
    map.Put(5, 4);
    REQUIRE(map.Get(5) == 4);

    REQUIRE_FALSE(map.GetPtr(7));
    REQUIRE(*map.GetPtr(5) == 4);

    map.Put(4141, 26);
    map.Put(874, 11);
    map.Put(95, 20);
    map.Put(66, 82);
    map.Put(10, 99);

    REQUIRE(map.Get(10) == 99);
    REQUIRE(map.GetSize() == 6);

    map[10] = 4;
    REQUIRE(map[10] == 4);
    REQUIRE(map.GetSize() == 6);

    map[12] = 12;
    REQUIRE(map[12] == 12);
    REQUIRE(map.GetSize() == 7);
}

TEST_CASE("Check if the hashmap contains an element.", "[HashMap::contains]")
{
    HashMap<uint32_t, uint32_t> map;

    map.Put(5, 8);
    map.Put(3, 4);

    REQUIRE(map.Contains(5));
    REQUIRE(map.Contains(3));

    REQUIRE(map.GetSize() == 2);
}

TEST_CASE("Check if the hashmap can remove an element.", "[HashMap::remove]")
{
    HashMap<uint32_t, uint32_t> map;

    map.Put(5, 8);
    map.Put(3, 4);

    REQUIRE(map.Contains(5));
    REQUIRE(map.GetSize() == 2);

    map.Remove(5);

    REQUIRE_FALSE(map.Contains(5));
    REQUIRE(map.GetSize() == 1);
}

TEST_CASE("Check is create correctly with an initiliazer list.", "[HashMap::HashMap]")
{
    HashMap<uint32_t, uint32_t> map = {
            {1, 2},
            {3, 20},
            {2, 2},
            {18, 50},
            {12, 10},
            {30, 20},
            {28, 111},
            {148, 147},
            {91, 954},
            {63, 454},
            {22, 4278},
            {198, 475},
    };

    REQUIRE(map.Contains(22));
    REQUIRE(map.GetSize() == 12);

    map.Remove(22);

    REQUIRE_FALSE(map.Contains(22));
    REQUIRE(map.GetSize() == 11);

    REQUIRE(map[30] == 20);
}

TEST_CASE("Copy and move semantics.", "[HashMap::copy_move]")
{
    HashMap<uint32_t, uint32_t> map;
    map.Put(1, 10);
    map.Put(2, 20);
    map.Put(3, 30);

    REQUIRE(map.Get(1) == 10);
    REQUIRE(map.Get(2) == 20);
    REQUIRE(map.Get(3) == 30);

    SECTION("copy constructor")
    {
        HashMap<uint32_t, uint32_t> mapCopy(map);
        REQUIRE(mapCopy.GetSize() == map.GetSize());
        REQUIRE(mapCopy.Get(1) == 10);
        REQUIRE(mapCopy.Get(2) == 20);
        REQUIRE(mapCopy.Get(3) == 30);
    }

    SECTION("copy assignment")
    {
        HashMap<uint32_t, uint32_t> mapAssign;
        mapAssign = map;
        REQUIRE(mapAssign.GetSize() == map.GetSize());
        REQUIRE(mapAssign.Get(1) == 10);
        REQUIRE(mapAssign.Get(2) == 20);
        REQUIRE(mapAssign.Get(3) == 30);
    }

    SECTION("move constructor")
    {
        HashMap<uint32_t, uint32_t> mapMove(map);
        HashMap<uint32_t, uint32_t> d(std::move(mapMove));
        REQUIRE(d.GetSize() == 3);
    }

    SECTION("move assignment")
    {
        HashMap<uint32_t, uint32_t> mapMoveAssign;
        mapMoveAssign = std::move(map);
        REQUIRE(mapMoveAssign.GetSize() == 3);
    }
}

TEST_CASE("Clear and operator at behavior.", "[HashMap::clear]")
{
    HashMap<uint32_t, uint32_t> map;
    map.Put(10, 100);
    map.Put(20, 200);

    REQUIRE(map.GetSize() == 2);

    map.Clear();
    REQUIRE(map.GetSize() == 0);

    // operator[] should insert a default value if missing
    uint32_t& v = map[50];
    REQUIRE(v == 0u);
    REQUIRE(map.GetSize() == 1);
    v = 55;
    REQUIRE(map[50] == 55u);
}

TEST_CASE("Iterator traversal and const iteration.", "[HashMap::Iterator]")
{
    HashMap<uint32_t, uint32_t> map;
    for (uint32_t i = 1; i <= 10; i++)
    {
        map.Put(i, i * 10);
    }

    Array<uint32_t> keys;
    Array<uint32_t> values;
    for (auto [key, value]: map)
    {
        keys.Append(key);
        values.Append(value);
    }

    REQUIRE(keys.GetSize() == map.GetSize());
    REQUIRE(values.GetSize() == map.GetSize());

    const HashMap<uint32_t, uint32_t>& cmap = map;
    size_t count = 0;
    for (HashMap<uint32_t, uint32_t>::const_iterator it = cmap.cbegin(); it != cmap.cend(); it++)
    {
        REQUIRE(cmap.Contains((*it).Key));
        ++count;
    }
    REQUIRE(count == cmap.GetSize());
}

TEST_CASE("Rehashing preserves entries for many inserts.", "[HashMap::resize_rehash]")
{
    HashMap<std::string, uint32_t> map(4);
    const uint32_t N = 200;

    for (uint32_t i = 0; i < N; i++)
    {
        std::string key = std::to_string(i);
        map.Put(key, i + 1000);
    }

    REQUIRE(map.GetSize() == N);

    for (uint32_t i = 0; i < N; i += 7)
    {
        std::string key = std::to_string(i);
        REQUIRE(map.Contains(key));
        REQUIRE(map.Get(key) == i + 1000);
    }
}

TEST_CASE("Works with non-trivial value types (Wl::String).", "[HashMap::put]")
{
    HashMap<String, String> map;
    map.Put("one", String("one"));
    map.Put("two", String("two"));
    map["three"] = String("three");
    map.Put("four", String(""));

    REQUIRE(map.Get("one") == "one");
    REQUIRE(map.Get("two") == "two");
    REQUIRE(map.Get("three") == "three");

    for (uint32_t i = 0; i < 150; i++)
    {
        String kv = Wl::Format("%u", i);
        map[kv] = kv;
    }

    REQUIRE(map["one"] == "one");
    REQUIRE(map["two"] == "two");
    REQUIRE(map["three"] == "three");

    for (uint32_t i = 0; i < 150; i++)
    {
        String kv = Wl::Format("%u", i);
        if (i == 4)
        {
            i = 4;
        }
        REQUIRE(map[kv] == kv);
    }

    String k122 = Wl::Format("%u", 122);
    map.Remove(k122);

    REQUIRE_FALSE(map.Contains(k122));

    String& s = map["four"];
    REQUIRE(s.IsEmpty());
    s = "four";
    REQUIRE(map["four"] == "four");

    const HashMap<String, String>& cmap = map;
    const String* p = cmap.GetPtr("two");
    REQUIRE(p);
    REQUIRE(*p == "two");
}

TEST_CASE("Overwrite value and repeated puts.", "[HashMap::put_overwrite]")
{
    HashMap<uint32_t, uint32_t> map;
    map.Put(42, 1);
    REQUIRE(map.Get(42) == 1);
    map.Put(42, 2);
    REQUIRE(map.Get(42) == 2);
    map.Put(42, 3);
    REQUIRE(map.Get(42) == 3);
    REQUIRE(map.GetSize() == 1);
}

TEST_CASE("Check nested  HashMap<String, HashMap<String, String>> behavior with put.", "[HashMap::HashMap]")
{
    HashMap<String, HashMap<String, String>> map;
    map.Put("one", HashMap<String, String>());
    map.Get("one").Put("one", "hundred");
    REQUIRE(map["one"]["one"] == "hundred");
}

TEST_CASE("Check nested  HashMap<uint32_t, HashMap<uint32_t, uint32_t>> behavior with operator[].", "[HashMap::HashMap]")
{
    HashMap<uint32_t, HashMap<uint32_t, uint32_t>> map;
    map[0] = HashMap<uint32_t, uint32_t>();
    map[0][1] = 100;
    REQUIRE(map[0][1] == 100);
}
