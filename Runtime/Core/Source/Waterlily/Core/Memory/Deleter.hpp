#pragma once

#include "Concepts.hpp"
#include "Waterlily/Core/Defines.hpp"
#include "Waterlily/Core/Memory/DefaultAllocator.hpp"
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

    template<typename ResourceType, CAlignedAllocator AllocatorType>
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
            Delete(m_allocator, resource);
        }

        Deleter(AllocatorType& allocator)
            : m_allocator(allocator)
        {
        }

        virtual ~Deleter() = default;

    private:
        AllocatorType& m_allocator;
    };

    template<typename ResourceType>
    using DefaultDeleter = Deleter<ResourceType, DefaultAllocator>;

    template<typename ResourceType>
    inline auto CreateDeleter(CAlignedAllocator auto& allocator) -> auto
    {
        return Deleter<ResourceType, decltype(allocator)>(allocator);
    }

}// namespace Wl
