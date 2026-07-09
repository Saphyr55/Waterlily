#pragma once

#include "Waterlily/Core/Memory/Concepts.hpp"
#include "Waterlily/Core/Memory/Allocator.hpp"
#include "Waterlily/Core/Memory/Memory.hpp"

namespace Wl
{

    template<typename DeleterType>
    class DeleterDelegate
    {
    public:
        template<typename ResourceType>
        void Invoke(ResourceType* resource) noexcept
        {
            m_deleter(resource);
        }

        explicit DeleterDelegate(const DeleterType& deleter)
            : m_deleter(deleter)
        {
        }

    private:
        DeleterType m_deleter;
    };

    template<typename ResourceType>
    class Deleter
    {
    public:
        void operator()(ResourceType* resource) noexcept
        {
            Destroy(resource);
        }

        void Destroy(ResourceType* resource) noexcept
        {
            WL_CHECK(resource);
            Wl::Delete(m_allocator, resource);
        }

        Deleter(Allocator* allocator)
            : m_allocator(allocator)
        {
        }

        virtual ~Deleter() = default;

    private:
        Allocator* m_allocator;
    };

    template<typename ResourceType, CAllocator AllocatorType>
    inline Deleter<ResourceType> CreateDeleter(AllocatorType& allocator)
    {
        return Deleter<ResourceType>(allocator);
    }

}// namespace Wl
