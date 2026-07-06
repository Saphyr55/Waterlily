#pragma once


#include "Waterlily/Core/Memory/Allocator.hpp"

namespace Wl
{

    template<typename ElementType>
    class TypedAllocator
    {
    public:
        template<typename OtherResourceType>
        struct rebind
        {
            using other = TypedAllocator<OtherResourceType>;
        };

    public:
        constexpr ElementType* Allocate(size_t n)
        {
            return static_cast<ElementType*>(m_allocator.Allocate(n * sizeof(ElementType), alignof(ElementType)));
        }

        constexpr void Deallocate(ElementType* element, size_t n)
        {
            m_allocator.Deallocate(element, n * sizeof(ElementType), alignof(ElementType));
        }

        constexpr ElementType* Allocate(size_t n, size_t alignment)
        {
            return static_cast<ElementType*>(m_allocator.Allocate(n * sizeof(ElementType), alignment));
        }

        constexpr void Deallocate(ElementType* element, size_t n, size_t alignment)
        {
            m_allocator.Deallocate(element, n * sizeof(ElementType), alignment);
        }

    protected:
        constexpr Allocator& GetAllocator()
        {
            return m_allocator;
        }

    public:
        constexpr TypedAllocator(Allocator& allocator)
            : m_allocator(allocator)
        {
        }
        constexpr ~TypedAllocator() = default;

    protected:
        Allocator& m_allocator;
    };

}// namespace Wl