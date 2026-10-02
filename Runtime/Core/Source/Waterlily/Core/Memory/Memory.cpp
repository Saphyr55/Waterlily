#include "Waterlily/Core/Memory/Memory.hpp"
#include "Waterlily/Core/Logging/Trace.hpp"
#include "Waterlily/Core/Memory/MemoryTrace.hpp"

#include <cstdlib>
#include <cstring>

namespace Wl
{

    uintptr_t Memory::AlignUp(uintptr_t address, usize alignment)
    {
        WL_CHECK_MSG(alignment > 0, "Alignment must be greater than zero.");
        WL_CHECK_MSG((alignment & (alignment - 1)) == 0, "Alignment must be a power of two.");

        return (address + (alignment - 1)) & ~(alignment - 1);
    }

    usize Memory::AlignAdjustment(const uintptr_t address, usize alignment)
    {
        WL_CHECK_MSG(alignment > 0, "Alignment must be greater than zero.");
        WL_CHECK_MSG((alignment & (alignment - 1)) == 0, "Alignment must be a power of two.");

        const uintptr_t mask = alignment - 1;
        const uintptr_t misalignment = address & mask;
        return misalignment > 0 ? alignment - misalignment : 0;
    }

    usize Memory::AlignedSize(usize size, usize alignment)
    {
        return (alignment - 1) + size;
    }

    bool Memory::IsAligned(uintptr_t address, usize alignment)
    {
        WL_CHECK_MSG(alignment > 0, "Alignment must be greater than zero.");
        WL_CHECK_MSG((alignment & (alignment - 1)) == 0, "Alignment must be a power of two.");

        return (address & (alignment - 1)) == 0;
    }

    uint8* Memory::Allocate(usize size) noexcept
    {
        Wl::MemoryTrace::GlobalAddAllocateByte(size);
        return static_cast<uint8*>(std::malloc(size));
    }

    uint8* Memory::Allocate(usize size, usize alignment) noexcept
    {
        WL_CHECK_MSG(alignment > 0, "Alignment must be greater than zero.");
        WL_CHECK_MSG((alignment & (alignment - 1)) == 0, "Alignment must be a power of two.");

        // To ensure we can align the memory, we allocate extra bytes.
        usize totalSize = size + alignment;
        uint8* raw = Memory::Allocate(totalSize);
        uint8* aligned = Align(raw, alignment);

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
        aligned[-1] = static_cast<uint8>(shift & 0xFF);

        return aligned;
    }

    void Memory::Deallocate(void* block, usize size) noexcept
    {
        MemoryTrace::GlobalAddDeallocatedByte(size);
        std::free(block);
    }

    void Memory::Deallocate(void* block, usize size, usize alignment) noexcept
    {
        usize totalSize = size + alignment;
        uint8* alignedMemory = static_cast<uint8*>(block);
        // Retrieve the shift value stored just before the aligned memory.
        ptrdiff_t shift = static_cast<ptrdiff_t>(alignedMemory[-1]);
        uint8* rawMemory = alignedMemory - shift;
        Deallocate(rawMemory, totalSize);
    }

    void* Memory::Copy(void* destination, const void* source, usize size)
    {
        WL_CHECK_MSG(destination, "The destination must be not a null pointer.");
        return std::memcpy(destination, source, size);
    }

    void* Memory::Move(void* destination, const void* source, usize size)
    {
        WL_CHECK_MSG(destination, "The destination must be not a null pointer.");
        return std::memmove(destination, source, size);
    }

    void* Memory::Write(void* destination, int32 value, usize size)
    {
        WL_CHECK_MSG(destination, "The destination must be not a null pointer.");
        return std::memset(destination, value, size);
    }

    int Memory::Compare(const void* buffer1, const void* buffer2, usize size)
    {
        return std::memcmp(buffer1, buffer2, size);
    }

}// namespace Wl
