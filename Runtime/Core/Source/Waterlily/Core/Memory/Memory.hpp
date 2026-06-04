#pragma once

#include <cstdint>
#include <type_traits>

#include "Waterlily/Core/CoreExports.hpp"
#include "Waterlily/Core/Defines.hpp"
#include "Waterlily/Core/Memory/Concepts.hpp"

#define WL_NEW ::new
#define WL_PLACEMENT_NEW WL_NEW
#define WL_KB (1024ul)
#define WL_MB (1024ul * WL_KB)
#define WL_GB (1024ul * WL_MB)

namespace Wl
{

    class Memory
    {
    public:
        /**
        * @brief Aligns the given address to the specified alignment.
        *
        * @param address The original memory address.
        * @param alignment The alignment boundary, in bytes.
        * @return The aligned memory address.
        */
        WL_CORE_API static uintptr_t AlignUp(uintptr_t address, size_t alignment);

        template<typename ResourceType>
        static ResourceType* Align(ResourceType* memory, size_t alignment)
        {
            return reinterpret_cast<ResourceType*>(AlignUp(reinterpret_cast<uintptr_t>(memory), alignment));
        }

        /**
        * @brief Calculates the adjustment needed to align the given address.
        *
        * @param address The original memory address.
        * @param alignment The alignment boundary, in bytes.
        * @return The number of bytes needed for alignment adjustment.
        */
        WL_CORE_API static size_t AlignAdjustment(const uintptr_t address, size_t alignment);

        /**
        * @brief Calculates the next aligned size based on the given the memory address size and alignment.
        *
        * @param size The size of a memory address.
        * @param alignment The alignment boundary, in bytes.
        * @return The next aligned size.
        */
        WL_CORE_API static size_t AlignedSize(size_t size, size_t alignment);

        /**
        * @brief Checks if a memory block address is aligned to the specified alignment.
        *
        * @param block A pointer to the memory block.
        * @param alignment The alignment boundary, in bytes.
        * @return `true` if the block is aligned, `false` otherwise.
        */
        WL_CORE_API static bool IsAligned(uintptr_t address, size_t alignment);

        /**
        * @brief Allocates a block of memory of the specified size.
        *
        * @param size The size of the memory block to allocate, in bytes.
        * @return A pointer to the allocated memory block.
        */
        WL_CORE_API static uint8_t* Allocate(size_t size) noexcept;
        WL_CORE_API static uint8_t* Allocate(size_t size, size_t alignment) noexcept;

        /**
        * @brief Frees a previously allocated block of memory.
        *
        * @param block A pointer to the memory block to free.
        * @param size The size of the memory block, in bytes.
        */
        WL_CORE_API static void Deallocate(void* block, size_t size) noexcept;
        WL_CORE_API static void Deallocate(void* block, size_t size, size_t alignment) noexcept;

        /**
        * @brief Writes a specified value into a memory block.
        *
        * @param destination A pointer to the memory block where the value will be written.
        * @param value The value to write into the memory block.
        * @param size The number of bytes to write.
        * @return A pointer to the destination memory block.
        */
        WL_CORE_API static void* Write(void* destination, int32_t value, size_t size);

        /**
        * @brief Copies a block of memory from the source to the destination.
        *
        * @param destination A pointer to the destination memory block.
        * @param source A pointer to the source memory block.
        * @param size The number of bytes to copy.
        * @return A pointer to the destination memory block.
        */
        WL_CORE_API static void* Copy(void* destination, const void* source, size_t size);

        WL_CORE_API static void* Move(void* destination, const void* source, size_t size);

        WL_CORE_API static int Compare(const void* buffer1, const void* buffer2, size_t size);

    public:
        Memory() = delete;
        Memory(const Memory&) = delete;
        Memory(Memory&&) = delete;
        ~Memory() = delete;

        Memory& operator=(Memory&&) = delete;
        Memory& operator=(const Memory&&) = delete;
    };

    template<typename ResourceType>
    inline ResourceType* New(Wl::CAlignedAllocator auto& allocator, ResourceType&& resource) noexcept
    {
        void* memory = allocator.Allocate(sizeof(ResourceType), alignof(ResourceType));
        if (!memory)
        {
            return nullptr;
        }
        WL_PLACEMENT_NEW(memory)
        ResourceType(std::move(resource));
        return static_cast<ResourceType*>(memory);
    }

    template<typename ResourceType, typename... Args>
        requires(std::is_constructible_v<ResourceType, Args && ...>)
    inline ResourceType* NewArgs(Wl::CAlignedAllocator auto& allocator, Args&&... args) noexcept
    {
        void* memory = allocator.Allocate(sizeof(ResourceType), alignof(ResourceType));
        if (!memory)
        {
            return nullptr;
        }
        WL_PLACEMENT_NEW(memory)
        ResourceType(std::forward<Args>(args)...);
        return static_cast<ResourceType*>(memory);
    }

    template<typename ResourceType>
    inline void Delete(Wl::CAlignedAllocator auto& allocator, ResourceType* ptr) noexcept
    {
        WL_CHECK(ptr);

        if constexpr (std::is_destructible_v<ResourceType>)
        {
            ptr->~ResourceType();
        }

        allocator.Deallocate(ptr, sizeof(ResourceType), alignof(ResourceType));
    }

}// namespace Wl
