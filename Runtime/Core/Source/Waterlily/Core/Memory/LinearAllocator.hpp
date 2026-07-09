#pragma once

#include "Waterlily/Core/CoreExports.hpp"
#include "Waterlily/Core/Memory/Allocator.hpp"

#include <cstdint>

namespace Wl
{

    class WL_CORE_API LinearAllocator : public Allocator
    {
    public:
        virtual void* Allocate(size_t size, size_t alignment = 1) override;

        virtual void Deallocate(void* block, size_t size, size_t alignment = 1) override;

    public:
        void Destroy();

        void Reset();

        inline size_t GetSize() const
        {
            return m_size;
        }

        inline size_t GetOffset() const
        {
            return m_offset;
        }

        inline bool IsValid() const
        {
            return m_buffer != nullptr;
        }

    public:
        LinearAllocator(Allocator* parent, size_t size);
        ~LinearAllocator();

    private:
        Allocator* m_parent;
        uint8_t* m_buffer;
        size_t m_size;
        size_t m_offset;
    };

}// namespace Wl
