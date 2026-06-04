#pragma once

#include "Waterlily/Core/Memory/DefaultAllocator.hpp"
#include "Waterlily/Core/Memory/Deleter.hpp"
#include "Waterlily/Core/Memory/ReferenceCounter.hpp"

#include <concepts>

namespace Wl
{

    template<typename ResourceType>
    class SharedPtr
    {
    public:
        ResourceType& operator*()
        {
            WL_CHECK(IsValid());
            return *m_resource;
        }

        ResourceType* operator->()
        {
            WL_CHECK(IsValid());
            return m_resource;
        }

        const ResourceType& operator*() const
        {
            WL_CHECK(IsValid());
            return *m_resource;
        }

        const ResourceType* operator->() const
        {
            WL_CHECK(IsValid());
            return m_resource;
        }

        ResourceType* GetResource() const
        {
            return m_resource;
        }

        ReferenceCounter* GetReferenceCounter()
        {
            return m_referenceCounter;
        }

        ReferenceCounter* GetReferenceCounter() const
        {
            return m_referenceCounter;
        }

        int32_t GetSharedReferenceCount() const
        {
            return m_referenceCounter->GetSharedReferenceCount();
        }

        bool IsUnique() const
        {
            return m_referenceCounter->IsUnique();
        }

        template<typename DeleterType = DefaultDeleter<ResourceType>>
        void Reset(ResourceType* resource = nullptr, DeleterType deleter = DeleterType())
        {
            ReleaseSharedReference();

            m_resource = resource;
            if (m_resource)
            {
                m_referenceCounter = NewRefCounterDeleter<ResourceType, DeleterType>(m_resource, deleter);
            }
        }

        bool IsValid() const
        {
            return m_resource != nullptr;
        }

        explicit operator bool() const
        {
            return IsValid();
        }

    public:
        SharedPtr() noexcept = default;

        SharedPtr(ResourceType* resource) noexcept
            : m_resource(resource)
            , m_referenceCounter(nullptr)
        {
            if (m_resource)
            {
                m_referenceCounter = NewDefaultRefCounter<ResourceType>(m_resource);
            }
        }

        SharedPtr(ResourceType* resource, ReferenceCounter* referenceCounter) noexcept
            : m_resource(resource)
            , m_referenceCounter(referenceCounter)
        {
            if (m_referenceCounter)
            {
                m_referenceCounter->AddSharedReference();
            }
        }

        template<typename DerivedType>
            requires std::derived_from<DerivedType, ResourceType>
        explicit SharedPtr(DerivedType* resource) noexcept
            : m_resource(resource)
        {
            if (m_resource)
            {
                m_referenceCounter = NewDefaultRefCounter<DerivedType>(m_resource);
            }
        }

        template<typename DeleterType>
        SharedPtr(ResourceType* resource, DeleterType deleter) noexcept
            : m_resource(resource)
        {
            if (m_resource)
            {
                m_referenceCounter = NewRefCounterDeleter<ResourceType, DeleterType>(m_resource, deleter);
            }
        }

        template<typename DerivedType, typename DeleterType>
            requires std::derived_from<DerivedType, ResourceType>
        SharedPtr(DerivedType* resource, DeleterType deleter) noexcept
            : m_resource(resource)
        {
            if (m_resource)
            {
                m_referenceCounter = NewRefCounterDeleter<DerivedType, DeleterType>(m_resource, deleter);
            }
        }

        SharedPtr(const SharedPtr& other)
            : m_resource(other.m_resource)
            , m_referenceCounter(other.m_referenceCounter)
        {
            if (m_referenceCounter)
            {
                m_referenceCounter->AddSharedReference();
            }
        }

        template<typename DerivedType>
            requires std::derived_from<DerivedType, ResourceType>
        SharedPtr(const SharedPtr<DerivedType>& other) noexcept
            : m_resource(other.m_resource)
            , m_referenceCounter(other.m_referenceCounter)
        {
            if (m_referenceCounter)
            {
                m_referenceCounter->AddSharedReference();
            }
        }

        SharedPtr& operator=(const SharedPtr& other) noexcept
        {
            ReleaseSharedReference();

            m_resource = static_cast<ResourceType*>(other.m_resource);
            m_referenceCounter = other.m_referenceCounter;

            if (m_referenceCounter)
            {
                m_referenceCounter->AddSharedReference();
            }

            return *this;
        }

        template<typename DerivedType>
            requires std::derived_from<DerivedType, ResourceType>
        SharedPtr& operator=(const SharedPtr<DerivedType>& other) noexcept
        {
            ReleaseSharedReference();

            m_resource = static_cast<ResourceType*>(other.m_resource);
            m_referenceCounter = other.m_referenceCounter;

            if (m_referenceCounter)
            {
                m_referenceCounter->AddSharedReference();
            }

            return *this;
        }

        SharedPtr(SharedPtr&& other) noexcept
            : m_resource(other.m_resource)
            , m_referenceCounter(other.m_referenceCounter)
        {
            other.m_resource = nullptr;
            other.m_referenceCounter = nullptr;
        }

        template<typename DerivedType>
            requires std::derived_from<DerivedType, ResourceType>
        SharedPtr(SharedPtr<DerivedType>&& other) noexcept
            : m_resource(other.m_resource)
            , m_referenceCounter(other.m_referenceCounter)
        {
            other.m_resource = nullptr;
            other.m_referenceCounter = nullptr;
        }

        SharedPtr& operator=(SharedPtr&& other) noexcept
        {
            ReleaseSharedReference();

            m_resource = other.m_resource;
            m_referenceCounter = other.m_referenceCounter;
            other.m_resource = nullptr;
            other.m_referenceCounter = nullptr;

            return *this;
        }

        template<typename DerivedType>
            requires std::derived_from<DerivedType, ResourceType>
        SharedPtr& operator=(SharedPtr<DerivedType>&& other) noexcept
        {
            ReleaseSharedReference();

            m_resource = other.m_resource;
            m_referenceCounter = other.m_referenceCounter;
            other.m_resource = nullptr;
            other.m_referenceCounter = nullptr;
            return *this;
        }

        template<typename DerivedType>
            requires std::derived_from<DerivedType, ResourceType>
        SharedPtr& operator=(DerivedType* newResource) noexcept
        {
            Reset(newResource);
            return *this;
        }

        ~SharedPtr() noexcept
        {
            ReleaseSharedReference();
        }

    private:
        void ReleaseSharedReference()
        {
            if (IsValid())
            {
                m_referenceCounter->ReleaseSharedReference();
            }
        }

        template<typename OtherResourceType>
        friend class SharedPtr;

    private:
        ResourceType* m_resource = nullptr;
        ReferenceCounter* m_referenceCounter = nullptr;
    };

    template<typename ResourceType, typename... Args>
        requires(std::is_constructible_v<ResourceType, Args && ...>)
    inline SharedPtr<ResourceType> MakeShared(Args&&... args) noexcept
    {
        return SharedPtr<ResourceType>(NewArgs<ResourceType>(DefaultAllocator::GetInstance(), args...));
    }

    template<typename ResourceType>
    bool operator==(const SharedPtr<ResourceType>& lhs, const SharedPtr<ResourceType>& rhs)
    {
        return lhs.GetResource() == rhs.GetResource();
    }

}// namespace Wl

template<typename ResourceType>
struct std::hash<Wl::SharedPtr<ResourceType>>
{
    size_t operator()(const Wl::SharedPtr<ResourceType>& GetRef) const
    {
        return std::hash<ResourceType*>()(GetRef.GetResource());
    }
};
