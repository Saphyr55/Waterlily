#include <catch2/catch_all.hpp>
#include <catch2/catch_approx.hpp>

#include "Waterlily/Core/Memory/LinearAllocator.hpp"

using namespace Wl;

TEST_CASE("LinearAllocator basic allocation", "[LinearAllocator]")
{
    LinearAllocator allocator;
    allocator.Init(1024);

    SECTION("Allocate returns non-null and respects alignment.")
    {
        void* ptr1 = allocator.Allocate(16, alignof(int32_t));
        REQUIRE(ptr1 != nullptr);
        REQUIRE(reinterpret_cast<std::uintptr_t>(ptr1) % alignof(int32_t) == 0);

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

    SECTION("Allocation beyond pool size fails with nullptr.")
    {
        void* big = allocator.Allocate(2000, alignof(int32_t));
        REQUIRE(big == nullptr);

        void* small = allocator.Allocate(64, alignof(int32_t));
        REQUIRE(small != nullptr);
    }

    allocator.Destroy();
}

TEST_CASE("TypedLinearAllocator usage.", "[TypedLinearAllocator]")
{
    LinearAllocator allocator(512);
    TypedLinearAllocator<int32_t> typedAllocator(&allocator);

    SECTION("Allocate array of ints.")
    {
        int32_t* arr = typedAllocator.Allocate(10);
        REQUIRE(arr != nullptr);

        for (int32_t i = 0; i < 10; i++)
        {
            arr[i] = i * 2;
        }
        for (int32_t i = 0; i < 10; i++)
        {
            REQUIRE(arr[i] == i * 2);
        }
    }

    SECTION("Allocate multiple different types with same pool.")
    {
        TypedLinearAllocator<double> float64Alloctorr(&allocator);

        int32_t* ints = typedAllocator.Allocate(4);
        REQUIRE(ints != nullptr);

        double* float64s = float64Alloctorr.Allocate(4);
        REQUIRE(float64s != nullptr);

        ints[0] = 7;
        float64s[0] = 3.1415;

        REQUIRE(ints[0] == 7);
        REQUIRE(float64s[0] == Catch::Approx(3.1415));
    }

    SECTION("Out of memory returns nullptr.")
    {
        int32_t* arr = typedAllocator.Allocate(10'000);
        REQUIRE(arr == nullptr);
    }
}