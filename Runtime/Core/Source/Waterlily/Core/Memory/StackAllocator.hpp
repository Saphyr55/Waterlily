#pragma once

#include "Waterlily/Core/CoreExports.hpp"
#include "Waterlily/Core/Defines.hpp"
#include "Waterlily/Core/Memory/Allocator.hpp"
#include "Waterlily/Core/Memory/TypedAllocator.hpp"

namespace Wl
{

    class WL_CORE_API StackAllocator : public AlignedAllocator
    {
    public:
        virtual void* Allocate(size_t size, size_t alignment) override;
        virtual void Deallocate(void* memory, size_t size, size_t alignment) override;

    public:
        inline void Reset()
        {
            m_head = 0;
        }

        inline void PopHead()
        {
            Pop(m_head);
        }

        void Pop(size_t marker)
        {
            WL_CHECK_MSG(marker <= m_size, "Marker is out of bounds.");
            m_head = marker - 1;
        }

        size_t GetHead() const
        {
            return m_head;
        }

        size_t GetSize() const
        {
            return m_size;
        }

    private:
        void Initialize(size_t size);
        void Destroy();

    public:
        explicit StackAllocator(size_t size);
        ~StackAllocator();

    private:
        size_t m_size;
        size_t m_head;
        uint8_t* m_buffer;
    };

    template<typename Type>
    class TypedStackAllocator : public TypedAllocator<Type, StackAllocator>
    {
        using Super = TypedAllocator<Type, StackAllocator>;

    public:
        void Reset()
        {
            Super::GetAllocator().Reset();
        }

        void PopHead()
        {
            Super::GetAllocator().PopHead();
        }

        void Pop(size_t marker)
        {
            Super::GetAllocator().Pop(marker);
        }
    };

}// namespace Wl