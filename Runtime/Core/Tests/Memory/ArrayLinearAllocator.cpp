#include <catch2/catch_all.hpp>

#include "Waterlily/Core/Containers/Array.hpp"
#include "Waterlily/Core/Memory/LinearAllocator.hpp"

using namespace Wl;

TEST_CASE("Array with LinearAllocator basic behavior.", "[Array][LinearAllocator]")
{
    struct IntPair
    {
        int32_t first;
        int32_t second;
    };

    LinearAllocator alloc(1024);// 1 KB

    SECTION("Construct array with linear allocator and Append elements.")
    {
        Array<int32_t, LinearAllocator> arr(alloc);

        REQUIRE(arr.GetSize() == 0);
        REQUIRE(arr.GetCapacity() >= 1);

        arr.Append(10);
        arr.Append(20);
        arr.Append(30);

        REQUIRE(arr.GetSize() == 3);
        REQUIRE(arr[0] == 10);
        REQUIRE(arr[1] == 20);
        REQUIRE(arr[2] == 30);
    }

    SECTION("Emplace and pop behave correctly.")
    {
        Array<IntPair, LinearAllocator> intPairArray(alloc);

        intPairArray.Emplace(1, 2);
        intPairArray.Emplace(3, 4);

        REQUIRE(intPairArray.GetSize() == 2);
        REQUIRE(intPairArray[0].first == 1);
        REQUIRE(intPairArray[0].second == 2);

        intPairArray.Pop();
        REQUIRE(intPairArray.GetSize() == 1);
        REQUIRE(intPairArray[0].first == 1);
    }

    SECTION("Copy and move semantics.")
    {
        Array<int32_t, LinearAllocator> arr(alloc);
        arr.Append(5);
        arr.Append(6);

        Array<int32_t, LinearAllocator> arrAssignCopy = arr;
        REQUIRE(arrAssignCopy.GetSize() == 2);
        REQUIRE(arrAssignCopy[0] == 5);
        REQUIRE(arrAssignCopy[1] == 6);

        Array<int32_t, LinearAllocator> arrAssignMove = std::move(arr);
        REQUIRE(arrAssignMove.GetSize() == 2);
        REQUIRE(arrAssignMove[0] == 5);
    }

    SECTION("Reserve, resize, shrink and clear.")
    {
        Array<int32_t, LinearAllocator> arr(alloc);

        arr.Reserve(16);
        REQUIRE(arr.GetCapacity() >= 16);

        arr.Resize(8, 42);
        REQUIRE(arr.size() == 8);

        for (size_t i = 0; i < arr.GetSize(); i++)
        {
            REQUIRE(arr[i] == 42);
        }

        arr.Shrink();
        REQUIRE(arr.GetCapacity() >= arr.GetSize());

        arr.Clear();
        REQUIRE(arr.GetSize() == 0);
    }

    SECTION("Check contains() and getIf() methods.")
    {
        Array<int32_t, LinearAllocator> arr(alloc);
        arr.Append(11);
        arr.Append(22);
        arr.Append(33);

        REQUIRE(arr.Contains(22));
        REQUIRE(!arr.Contains(99));

        int32_t* found = arr.GetIf([](int32_t v)
        {
            return v == 33;
        });
        REQUIRE(found != nullptr);
        REQUIRE(*found == 33);
    }

    alloc.Reset();
}
