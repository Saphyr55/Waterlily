#pragma once

#include "Waterlily/Core/Asserts.hpp"
#include "Waterlily/Core/CoreExports.hpp"
#include "Waterlily/Core/Memory/Allocator.hpp"

namespace Wl
{

    class WL_CORE_API StackAllocator : public Allocator
    {
    public:
        virtual void* Allocate(usize size, usize alignment) override;
        virtual void Deallocate(void* memory, usize size, usize alignment) override;

    public:
        inline void Reset()
        {
            m_head = 0;
        }

        inline void PopHead()
        {
            Pop(m_head);
        }

        void Pop(usize marker)
        {
            WL_CHECK(marker <= m_size);
            m_head = marker - 1;
        }

        usize GetHead() const
        {
            return m_head;
        }

        usize GetSize() const
        {
            return m_size;
        }

    private:
        void Initialize(usize size);
        void Destroy();

    public:
        explicit StackAllocator(usize size);
        ~StackAllocator();

    private:
        usize m_size;
        usize m_head;
        uint8* m_buffer;
    };

}// namespace Wl
