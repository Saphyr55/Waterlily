#include "Waterlily/Core/Memory/Memory.hpp"
#include "Waterlily/Core/Defines.hpp"
#include "Waterlily/Core/Memory/MemoryTrace.hpp"

#include <cstdlib>
#include <cstring>

namespace Wl
{

    uintptr_t Memory::AlignUp(uintptr_t address, size_t alignment)
    {
        WL_CHECK_MSG(alignment > 0, "Alignment must be greater than zero.");
        WL_CHECK_MSG((alignment & (alignment - 1)) == 0, "Alignment must be a power of two.");

        return (address + (alignment - 1)) & ~(alignment - 1);
    }

    size_t Memory::AlignAdjustment(const uintptr_t address, size_t alignment)
    {
        WL_CHECK_MSG(alignment > 0, "Alignment must be greater than zero.");
        WL_CHECK_MSG((alignment & (alignment - 1)) == 0, "Alignment must be a power of two.");

        const uintptr_t mask = alignment - 1;
        const uintptr_t misalignment = address & mask;
        return misalignment > 0 ? alignment - misalignment : 0;
    }

    size_t Memory::AlignedSize(size_t size, size_t alignment)
    {
        return (alignment - 1) + size;
    }

    bool Memory::IsAligned(uintptr_t address, size_t alignment)
    {
        WL_CHECK_MSG(alignment > 0, "Alignment must be greater than zero.");
        WL_CHECK_MSG((alignment & (alignment - 1)) == 0, "Alignment must be a power of two.");

        return (address & (alignment - 1)) == 0;
    }

    uint8_t* Memory::Allocate(size_t size) noexcept
    {
        Wl::MemoryTrace::GlobalAddAllocateByte(size);
        return static_cast<uint8_t*>(::malloc(size));
    }

    uint8_t* Memory::Allocate(size_t size, size_t alignment) noexcept
    {
        WL_CHECK_MSG(alignment > 0, "Alignment must be greater than zero.");
        WL_CHECK_MSG((alignment & (alignment - 1)) == 0, "Alignment must be a power of two.");

        // To ensure we can align the memory, we allocate extra bytes.
        size_t totalSize = size + alignment;
        uint8_t* raw = Memory::Allocate(totalSize);
        uint8_t* aligned = Align(raw, alignment);

        if (aligned == raw)
        {
            // If the memory is already aligned, we need to move the pointer forward
            // to ensure we have enough space for alignment.
            aligned += alignment;
        }

        // Calculate the shift for debugging purposes.
        ptrdiff_t shift = aligned - raw;
        WL_CHECK_MSG(shift >= 0 && shift <= alignment, "Alignment adjustment is out of bounds.");

        // Store the shift value just before the aligned memory for later use in deallocation.
        aligned[-1] = static_cast<uint8_t>(shift & 0xFF);

        return aligned;
    }

    void Memory::Deallocate(void* block, size_t size) noexcept
    {
        MemoryTrace::GlobalAddDeallocatedByte(size);
        std::free(block);
    }

    void Memory::Deallocate(void* block, size_t size, size_t alignment) noexcept
    {
        size_t totalSize = size + alignment;
        uint8_t* alignedMemory = static_cast<uint8_t*>(block);
        // Retrieve the shift value stored just before the aligned memory.
        ptrdiff_t shift = static_cast<ptrdiff_t>(alignedMemory[-1]);
        uint8_t* rawMemory = alignedMemory - shift;
        Deallocate(rawMemory, totalSize);
    }

    void* Memory::Copy(void* destination, const void* source, size_t size)
    {
        WL_CHECK_MSG(destination, "The destination must be not a null pointer.");
        return std::memcpy(destination, source, size);
    }

    void* Memory::Move(void* destination, const void* source, size_t size)
    {
        WL_CHECK_MSG(destination, "The destination must be not a null pointer.");
        return std::memmove(destination, source, size);
    }

    void* Memory::Write(void* destination, int32_t value, size_t size)
    {
        WL_CHECK_MSG(destination, "The destination must be not a null pointer.");
        return std::memset(destination, value, size);
    }

    int Memory::Compare(const void* buffer1, const void* buffer2, size_t size)
    {
        return std::memcmp(buffer1, buffer2, size);
    }

}// namespace Wl
