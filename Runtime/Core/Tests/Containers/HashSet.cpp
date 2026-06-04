#include <catch2/catch_all.hpp>

#include "Waterlily/Core/Containers/HashSet.hpp"
#include "Waterlily/Core/Defines.hpp"
#include "Waterlily/Core/String/String.hpp"

using namespace Wl;

TEST_CASE("HashSet - Constructors and Initial State", "[HashSet]")
{
    SECTION("Default Constructor")
    {
        HashSet<int32_t> set;
        REQUIRE(set.IsEmpty());
        REQUIRE(set.GetSize() == 0);
        REQUIRE(set.GetCapacity() == 8);
    }

    SECTION("Constructor with Initial Capacity")
    {
        HashSet<String> set(16);
        REQUIRE(set.GetCapacity() == 16);
    }

    SECTION("Constructor with Initializer List")
    {
        HashSet<int32_t> set = {10, 20, 30, 20, 40};
        REQUIRE_FALSE(set.IsEmpty());
        REQUIRE(set.GetSize() == 4);
        REQUIRE(set.Contains(10));
        REQUIRE(set.Contains(30));
    }
}

TEST_CASE("HashSet - Insertion and Uniqueness", "[HashSet][Insert]")
{
    HashSet<int32_t> set;

    SECTION("Inserting New Keys")
    {
        HashSet<int32_t>::Iterator it1 = set.Insert(100);
        REQUIRE(*it1 == 100);
        REQUIRE(set.GetSize() == 1);

        HashSet<int32_t>::Iterator it2 = set.Insert(200);
        REQUIRE(*it2 == 200);
        REQUIRE(set.GetSize() == 2);
    }

    SECTION("Inserting Existing Keys (Uniqueness)")
    {
        set.Insert(50);
        REQUIRE(set.GetSize() == 1);

        HashSet<int32_t>::Iterator it = set.Insert(50);
        REQUIRE(*it == 50);
        REQUIRE(set.GetSize() == 1);
    }
}

TEST_CASE("HashSet - Containment and Lookup", "[HashSet][Contains][Find]")
{
    HashSet<String> set = {"apple", "banana", "orange"};

    SECTION("Existing Key")
    {
        REQUIRE(set.Contains("apple"));
        REQUIRE(set.find("banana") != set.end());
    }

    SECTION("Non-Existing Key")
    {
        REQUIRE_FALSE(set.Contains("strawberry"));
        REQUIRE(set.find("strawberry") == set.end());
    }

    SECTION("Lookup in an Empty Set")
    {
        HashSet<int32_t> empty_set;
        REQUIRE_FALSE(empty_set.Contains(1));
        REQUIRE(empty_set.find(1) == empty_set.end());
    }
}

TEST_CASE("HashSet - Removal", "[HashSet][Remove]")
{
    HashSet<int32_t> set = {1, 2, 3, 4};
    REQUIRE(set.GetSize() == 4);

    SECTION("Remove Existing Key")
    {
        size_t count = set.Remove(3);
        REQUIRE(count == 1);
        REQUIRE(set.GetSize() == 3);
        REQUIRE_FALSE(set.Contains(3));

        count = set.Remove(1);
        REQUIRE(count == 1);
        REQUIRE(set.GetSize() == 2);
        REQUIRE_FALSE(set.Contains(1));
    }

    SECTION("Remove Non-Existing Key")
    {
        size_t count = set.Remove(99);
        REQUIRE(count == 0);
        REQUIRE(set.GetSize() == 4);
    }

    SECTION("Remove Until Empty")
    {
        set.Remove(1);
        set.Remove(2);
        set.Remove(3);
        set.Remove(4);
        REQUIRE(set.IsEmpty());
        REQUIRE(set.GetSize() == 0);
    }
}

TEST_CASE("HashSet - Copy and Move Semantics", "[HashSet][Copy][Move]")
{
    HashSet<String> original = {"a", "b", "c"};

    SECTION("Copy Constructor")
    {
        HashSet<String> copy = original;

        REQUIRE(copy.GetSize() == 3);
        REQUIRE(copy.Contains("a"));

        original.Remove("a");
        REQUIRE(original.GetSize() == 2);
        REQUIRE(copy.GetSize() == 3);
    }

    SECTION("Copy Assignment Operator")
    {
        HashSet<String> assign_copy;
        assign_copy = original;

        REQUIRE(assign_copy.GetSize() == 3);
        REQUIRE(assign_copy.Contains("b"));

        original.Add("d");
        REQUIRE_FALSE(assign_copy.Contains("d"));
    }

    SECTION("Move Constructor")
    {
        HashSet<String> moved = std::move(original);

        REQUIRE(moved.GetSize() == 3);
        REQUIRE(moved.Contains("c"));
        REQUIRE(original.IsEmpty());
    }

    SECTION("Move Assignment Operator")
    {
        HashSet<String> target = {"x", "y"};
        target = std::move(original);

        REQUIRE(target.GetSize() == 3);
        REQUIRE(target.Contains("a"));
        REQUIRE(original.IsEmpty());
    }
}

TEST_CASE("HashSet - Iteration and Clear", "[HashSet][Iterators][Clear]")
{
    HashSet<int32_t> set = {10, 20, 30, 40};

    SECTION("Basic Iteration (begin/end)")
    {
        Array<int32_t> elements;
        for (const int32_t& key: set)
        {
            elements.push_back(key);
        }
        REQUIRE(elements.Contains(30));
        REQUIRE(elements.GetSize() == 4);
    }

    SECTION("Iteration on an Empty Set")
    {
        HashSet<int32_t> empty_set;
        int32_t count = 0;
        for (auto it = empty_set.begin(); it != empty_set.end(); ++it)
        {
            count++;
        }
        REQUIRE(count == 0);
    }

    SECTION("Clear Method")
    {
        REQUIRE_FALSE(set.IsEmpty());
        size_t cap = set.GetCapacity();
        set.Clear();

        REQUIRE(set.IsEmpty());
        REQUIRE(set.GetSize() == 0);
        REQUIRE(set.GetCapacity() == cap);
        REQUIRE_FALSE(set.Contains(20));
    }
}

TEST_CASE("HashSet - Complex Types", "[HashSet][String]")
{
    HashSet<String> set;

    SECTION("Inserting Strings")
    {
        set.Insert("hello");
        set.Insert("world");
        set.Add("hello");

        REQUIRE(set.GetSize() == 2);
        REQUIRE(set.Contains("hello"));
        REQUIRE(set.Contains("world"));
    }

    SECTION("Removing Strings")
    {
        set.Insert("key1");
        String& key2 = set.Add("key2");
        REQUIRE(set.GetSize() == 2);
        REQUIRE(key2 == "key2");

        set.Remove("key1");
        REQUIRE(set.GetSize() == 1);
        REQUIRE_FALSE(set.Contains("key1"));
        REQUIRE(set.Contains("key2"));
    }
}
