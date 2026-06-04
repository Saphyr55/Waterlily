#pragma once


#include "Waterlily/Core/Memory/Concepts.hpp"

namespace Wl
{

    template<typename ElementType, CAlignedAllocator AllocatorType>
    class TypedAllocator
    {
    public:
        template<typename OtherResourceType, CAlignedAllocator OtherAllocatorType = AllocatorType>
        struct rebind
        {
            using other = TypedAllocator<OtherResourceType, OtherAllocatorType>;
        };

    public:
        constexpr ElementType* Allocate(size_t n)
        {
            return static_cast<ElementType*>(m_allocator->Allocate(n * sizeof(ElementType), alignof(ElementType)));
        }

        constexpr void Deallocate(ElementType* element, size_t n)
        {
            m_allocator->Deallocate(element, n * sizeof(ElementType), alignof(ElementType));
        }

        constexpr ElementType* Allocate(size_t n, size_t alignment)
        {
            return static_cast<ElementType*>(m_allocator->Allocate(n * sizeof(ElementType), alignment));
        }

        constexpr void Deallocate(ElementType* element, size_t n, size_t alignment)
        {
            m_allocator->Deallocate(element, n * sizeof(ElementType), alignment);
        }

        constexpr bool IsValid() const
        {
            return m_allocator != nullptr;
        }

    protected:
        constexpr AllocatorType* GetAllocator()
        {
            return m_allocator;
        }

    public:
        constexpr TypedAllocator(AllocatorType* memory)
            : m_allocator(memory)
        {
        }
        constexpr ~TypedAllocator() = default;

    protected:
        AllocatorType* m_allocator;
    };

}// namespace Wl