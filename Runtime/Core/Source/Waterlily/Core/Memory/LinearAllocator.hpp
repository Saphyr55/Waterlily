#pragma once

#include "Waterlily/Core/CoreExports.hpp"
#include "Waterlily/Core/Memory/Allocator.hpp"

#include <cstdint>

namespace Wl
{

    class WL_CORE_API LinearAllocator : public Allocator
    {
    public:
        virtual void* Allocate(usize size, usize alignment = 1) override;

        virtual void Deallocate(void* block, usize size, usize alignment = 1) override;

    public:
        void Destroy();

        void Reset();

        inline usize GetSize() const
        {
            return m_size;
        }

        inline usize GetOffset() const
        {
            return m_offset;
        }

        inline bool IsValid() const
        {
            return m_buffer != nullptr;
        }

    public:
        LinearAllocator(Allocator* parent, usize size);
        ~LinearAllocator();

    private:
        Allocator* m_parent;
        uint8* m_buffer;
        usize m_size;
        usize m_offset;
    };

}// namespace Wl
