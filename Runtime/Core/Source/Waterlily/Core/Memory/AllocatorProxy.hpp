#pragma once

#include "Waterlily/Core/Memory/Allocator.hpp"
#include "Waterlily/Core/Memory/DefaultAllocator.hpp"

namespace Wl
{

    class AllocatorProxy : public Allocator
    {
    public:
        inline virtual void* Allocate(size_t size, size_t alignment) override;

        inline virtual void Deallocate(void* memory, size_t size, size_t alignment) override;

        inline Allocator* GetDelegate() const;

        inline void SetDelegate(Allocator* allocator);

    public:
        AllocatorProxy() = default;
        AllocatorProxy(const AllocatorProxy&) = default;
        AllocatorProxy(AllocatorProxy&&) = default;
        ~AllocatorProxy() = default;

        AllocatorProxy& operator=(AllocatorProxy&& other) = default;
        AllocatorProxy& operator=(const AllocatorProxy& other) = default;

    private:
        Allocator* m_allocator = &DefaultAllocator::GetInstance();
    };

    inline void* AllocatorProxy::Allocate(size_t size, size_t alignment)
    {
        return m_allocator->Allocate(size, alignment);
    }

    inline void AllocatorProxy::Deallocate(void* memory, size_t size, size_t alignment)
    {
        m_allocator->Deallocate(memory, size, alignment);
    }

    inline Allocator* AllocatorProxy::GetDelegate() const
    {
        return m_allocator;
    }

    inline void AllocatorProxy::SetDelegate(Allocator* allocator)
    {
        m_allocator = allocator;
    }

}// namespace Wl
