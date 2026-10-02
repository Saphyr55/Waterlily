#include "Waterlily/Core/Memory/LinearAllocator.hpp"
#include "Waterlily/Core/Memory/HeapAllocator.hpp"


#include <catch2/catch_all.hpp>
#include <catch2/catch_approx.hpp>


using namespace Wl;

static HeapAllocator heap;

TEST_CASE("LinearAllocator basic allocation", "[LinearAllocator]")
{
    LinearAllocator allocator(&heap, 1 * WL_KB);

    SECTION("Allocate returns non-null and respects alignment.")
    {
        void* ptr1 = allocator.Allocate(16, alignof(int32));
        REQUIRE(ptr1 != nullptr);
        REQUIRE(reinterpret_cast<std::uintptr_t>(ptr1) % alignof(int32) == 0);

        void* ptr2 = allocator.Allocate(32, alignof(double));
        REQUIRE(ptr2 != nullptr);
        REQUIRE(reinterpret_cast<std::uintptr_t>(ptr2) % alignof(double) == 0);
    }

    SECTION("Reset allows reuse of memory.")
    {
        double* ptr1 = static_cast<double*>(allocator.Allocate(sizeof(double), alignof(double)));
        REQUIRE(ptr1 != nullptr);
        *ptr1 = 42.0;

        allocator.Reset();

        double* ptr2 = static_cast<double*>(allocator.Allocate(sizeof(double), alignof(double)));
        REQUIRE(ptr2 != nullptr);
        *ptr2 = 99.0;

        REQUIRE(*ptr2 == 99.0);
    }

    allocator.Destroy();
}

TEST_CASE("TypedLinearAllocator usage.", "[TypedLinearAllocator]")
{
    LinearAllocator allocator(&heap, 512);

    SECTION("Allocate array of ints.")
    {
        int32* arr = static_cast<int32*>(allocator.Allocate(10 * sizeof(int32)));
        REQUIRE(arr != nullptr);

        for (int32 i = 0; i < 10; i++)
        {
            arr[i] = i * 2;
        }
        for (int32 i = 0; i < 10; i++)
        {
            REQUIRE(arr[i] == i * 2);
        }
    }

    SECTION("Allocate multiple different types with same pool.")
    {
        int32* ints = static_cast<int32*>(allocator.Allocate(4 * sizeof(int32)));
        REQUIRE(ints != nullptr);

        double* doubles = static_cast<double*>(allocator.Allocate(4 * sizeof(double)));
        REQUIRE(doubles != nullptr);

        ints[0] = 7;
        doubles[0] = 3.1415;

        REQUIRE(ints[0] == 7);
        REQUIRE(doubles[0] == Catch::Approx(3.1415));
    }
}
