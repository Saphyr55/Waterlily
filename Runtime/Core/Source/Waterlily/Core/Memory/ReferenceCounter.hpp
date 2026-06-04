#pragma once

#include "Waterlily/Core/CoreExports.hpp"
#include "Waterlily/Core/Defines.hpp"
#include "Waterlily/Core/Memory/DefaultAllocator.hpp"
#include "Waterlily/Core/Memory/Deleter.hpp"
#include "Waterlily/Core/Memory/Memory.hpp"

#include <atomic>
#include <utility>

namespace Wl
{

    class WL_CORE_API ReferenceCounter
    {
    public:
        using RefCountType = std::atomic_int32_t;

    public:
        ReferenceCounter() = default;
        virtual ~ReferenceCounter() = default;

        virtual void DestroyResource() = 0;

        int32_t GetSharedReferenceCount() const
        {
            return m_sharedReferenceCount.load(std::memory_order_relaxed);
        }

        bool IsUnique() const
        {
            return GetSharedReferenceCount() == 1;
        }

        void AddSharedReference()
        {
            m_sharedReferenceCount.fetch_add(1, std::memory_order_relaxed);
        }

        void ReleaseSharedReference()
        {
            int32_t prevCount = m_sharedReferenceCount.fetch_sub(1, std::memory_order_relaxed);

            if (prevCount == 1)
            {
                // Ensure destruction and deletion are synchronized.
                std::atomic_thread_fence(std::memory_order_acquire);

                DestroyResource();
                Wl::Delete(DefaultAllocator::GetInstance(), this);
            }
        }

    protected:
        RefCountType m_sharedReferenceCount = 1;
    };

    template<typename ResourceType, typename DeleterType>
    class ReferenceCounterWithDeleter : private DeleterDelegate<DeleterType>
        , public ReferenceCounter
    {
    public:
        virtual void DestroyResource() override
        {
            if (m_resource)
            {
                this->Invoke(m_resource);
                m_resource = nullptr;
            }
        }

        ReferenceCounterWithDeleter(ResourceType* resource, const DeleterType& deleter)
            : DeleterDelegate<DeleterType>(deleter)
            , m_resource(resource)
        {
            WL_CHECK(m_resource);
        }

        ReferenceCounterWithDeleter(const ReferenceCounterWithDeleter&) = delete;
        ReferenceCounterWithDeleter& operator=(const ReferenceCounterWithDeleter&) = delete;

        ReferenceCounterWithDeleter(ReferenceCounterWithDeleter&& other) noexcept
            : DeleterDelegate<DeleterType>(std::move(other))
            , m_resource(std::exchange(other.m_resource, nullptr))
        {
        }

        ReferenceCounterWithDeleter& operator=(ReferenceCounterWithDeleter&& other) noexcept
        {
            if (this != &other)
            {
                DestroyResource();

                DeleterDelegate<DeleterType>::operator=(std::move(other));
                m_resource = std::exchange(other.m_resource, nullptr);
            }
            return *this;
        }

    private:
        ResourceType* m_resource = nullptr;
    };

    template<typename ResourceType, typename DeleterType>
    inline ReferenceCounter* NewRefCounterDeleter(ResourceType* resource, const DeleterType& deleter) noexcept
    {
        using RefCounter = ReferenceCounterWithDeleter<ResourceType, DeleterType>;
        return NewArgs<RefCounter>(DefaultAllocator::GetInstance(), resource, deleter);
    }

    template<typename ResourceType>
    inline ReferenceCounter* NewDefaultRefCounter(ResourceType* resource) noexcept
    {
        using DeleterType = DefaultDeleter<ResourceType>;
        return NewRefCounterDeleter(resource, DeleterType(DefaultAllocator::GetInstance()));
    }

}// namespace Wl