#include "Waterlily/Core/Memory/Memory.hpp"

#include <catch2/catch_all.hpp>
#include <catch2/catch_approx.hpp>

using namespace Wl;

TEST_CASE("Memory alignment functions.", "[Memory][Alignment]")
{
    SECTION("AlignUp() calculates correct alignment.")
    {
        REQUIRE(Memory::AlignUp(0, 16) == 0);
        REQUIRE(Memory::AlignUp(1, 16) == 16);
        REQUIRE(Memory::AlignUp(15, 16) == 16);
        REQUIRE(Memory::AlignUp(16, 16) == 16);
        REQUIRE(Memory::AlignUp(17, 16) == 32);
    }

    SECTION("AlignAdjustment() calculates correct adjustment.")
    {
        REQUIRE(Memory::AlignAdjustment(0, 16) == 0);
        REQUIRE(Memory::AlignAdjustment(1, 16) == 15);
        REQUIRE(Memory::AlignAdjustment(15, 16) == 1);
        REQUIRE(Memory::AlignAdjustment(16, 16) == 0);
        REQUIRE(Memory::AlignAdjustment(17, 16) == 15);
    }

    SECTION("AlignedSize() calculates correct size with alignment.")
    {
        REQUIRE(Memory::AlignedSize(10, 16) == 25);// (16-1) + 10 = 15 + 10 = 25
        REQUIRE(Memory::AlignedSize(16, 16) == 31);// (16-1) + 16 = 15 + 16 = 31
    }

    SECTION("IsAligned() correctly detects alignment.")
    {
        REQUIRE(Memory::IsAligned(0, 16) == true);
        REQUIRE(Memory::IsAligned(16, 16) == true);
        REQUIRE(Memory::IsAligned(32, 16) == true);
        REQUIRE(Memory::IsAligned(1, 16) == false);
        REQUIRE(Memory::IsAligned(15, 16) == false);
        REQUIRE(Memory::IsAligned(17, 16) == false);
    }
}

TEST_CASE("Memory allocation and deallocation.", "[Memory][Allocation]")
{
    SECTION("Basic allocation and free.")
    {
        void* ptr = Memory::Allocate(100);
        REQUIRE(ptr != nullptr);
        Memory::Deallocate(ptr, 100);
    }

    SECTION("Aligned allocation and free.")
    {
        const size_t alignment = 16;
        const size_t size = 100;

        void* ptr = Memory::Allocate(size, alignment);
        REQUIRE(ptr != nullptr);

        uintptr_t address = reinterpret_cast<uintptr_t>(ptr);
        REQUIRE(Memory::IsAligned(address, alignment));

        Memory::Deallocate(ptr, size, alignment);
    }

    SECTION("Aligned allocation with various alignments.")
    {
        const size_t sizes[] = {10, 100, 1024};
        const size_t alignments[] = {2, 4, 8, 16, 32, 64, 128};

        for (size_t size: sizes)
        {
            for (size_t alignment: alignments)
            {
                void* ptr = Memory::Allocate(size, alignment);
                REQUIRE(ptr != nullptr);

                uintptr_t address = reinterpret_cast<uintptr_t>(ptr);
                REQUIRE(Memory::IsAligned(address, alignment));

                Memory::Deallocate(ptr, size, alignment);
            }
        }
    }
}

TEST_CASE("Memory utility functions.", "[Memory][Utility]")
{
    SECTION("copy() function works correctly.")
    {
        constexpr const char source[] = "Hello, World!";
        char destination[20];

        void* result = Memory::Copy(destination, source, sizeof(source));
        REQUIRE(result == destination);
        REQUIRE(Memory::Compare(destination, source, sizeof(source)) == 0);
    }

    SECTION("CopyMove() function works correctly.")
    {
        char buffer[] = "Hello, World!";

        void* result = Memory::Move(buffer + 6, buffer, 7);
        REQUIRE(result == buffer + 6);
        REQUIRE(std::strcmp(buffer + 6, "Hello, ") == 0);
    }

    SECTION("write() function works correctly.")
    {
        constexpr size_t size = 10;
        char buffer[size];

        void* result = Memory::Write(buffer, int32_t('A'), size);
        REQUIRE(result == buffer);

        for (int8_t i = 0; i < size; i++)
        {
            REQUIRE(buffer[i] == 0x41);
        }
    }

    SECTION("compare() function works correctly.")
    {
        const char str1[] = "Hello";
        const char str2[] = "Hello";
        const char str3[] = "World";

        REQUIRE(Memory::Compare(str1, str2, 5) == 0);
        REQUIRE(Memory::Compare(str1, str3, 5) < 0);
        REQUIRE(Memory::Compare(str3, str1, 5) > 0);
    }
}

TEST_CASE("Edge cases and stress tests", "[Memory][Edge]")
{
    SECTION("Zero size allocation.")
    {
        void* ptr = Memory::Allocate(0);
        REQUIRE(ptr != nullptr);
        Memory::Deallocate(ptr, 0);
    }

    SECTION("Large allocation.")
    {
        constexpr size_t large_size = 1024 * 1024;// 1MB
        void* ptr = Memory::Allocate(large_size);
        REQUIRE(ptr != nullptr);
        Memory::Deallocate(ptr, large_size);
    }

    SECTION("Maximum alignment.")
    {
        constexpr size_t max_alignment = 128;
        void* ptr = Memory::Allocate(100, max_alignment);
        REQUIRE(ptr != nullptr);
        REQUIRE(Memory::IsAligned(reinterpret_cast<uintptr_t>(ptr), max_alignment));
        Memory::Deallocate(ptr, 100, max_alignment);
    }

    SECTION("Multiple allocations and deallocations.")
    {
        constexpr int num_allocations = 100;
        void* pointers[num_allocations];

        for (int i = 0; i < num_allocations; i++)
        {
            pointers[i] = Memory::Allocate(i + 1);
            REQUIRE(pointers[i] != nullptr);
        }

        for (int i = 0; i < num_allocations; i++)
        {
            Memory::Deallocate(pointers[i], i + 1);
        }
    }
}
