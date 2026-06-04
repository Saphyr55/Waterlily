#pragma once

#include "Waterlily/Core/Containers/Array.hpp"
#include "Waterlily/Core/Containers/HashMap.hpp"
#include "Waterlily/Core/Function/Function.hpp"

namespace Wl
{

    template<typename KeyType, typename ObjectType, typename HashType = Hash<KeyType>>
    class ObjectCache
    {
    public:
        using IndexType = size_t;
        using EntryType = Entry<KeyType, IndexType>;
        using CacheType = HashMap<KeyType, IndexType, HashType>;
        using RegistryType = Array<ObjectType>;

        using CreateCallbackType = Function<ObjectType(const KeyType&)>;
        using DestroyCallbackType = Function<void(ObjectType&)>;

    public:
        ObjectType& Obtain(const KeyType& key);

        RegistryType& GetResources();
        const RegistryType& GetResources() const;

        void Dispose();

    public:
        ObjectCache() = default;
        ~ObjectCache();

    protected:
        CacheType m_cache;
        RegistryType m_registry;
        CreateCallbackType m_createCallback;
        DestroyCallbackType m_destroyCallback;
    };

    template<typename KeyType, typename ObjectType, typename HashType>
    inline ObjectType& ObjectCache<KeyType, ObjectType, HashType>::Obtain(const KeyType& key)
    {
        typename CacheType::iterator it = m_cache.Find(key);
        if (it != m_cache.end())
        {
            return m_registry[(*it).Value];
        }

        ObjectType resource = m_createCallback(key);
        m_registry.Append(resource);

        IndexType index = m_registry.GetSize() - 1;
        m_cache.Put(key, index);

        return m_registry[index];
    }

    template<typename KeyType, typename ObjectType, typename HashType>
    inline Array<ObjectType>& ObjectCache<KeyType, ObjectType, HashType>::GetResources()
    {
        return m_registry;
    }

    template<typename KeyType, typename ObjectType, typename HashType>
    inline const Array<ObjectType>& ObjectCache<KeyType, ObjectType, HashType>::GetResources() const
    {
        return m_registry;
    }

    template<typename KeyType, typename ResourceType, typename HashType>
    inline void ObjectCache<KeyType, ResourceType, HashType>::Dispose()
    {
        for (ResourceType& resource: m_registry)
        {
            m_destroyCallback(resource);
        }
    }

    template<typename KeyType, typename ResourceType, typename HashType>
    inline ObjectCache<KeyType, ResourceType, HashType>::~ObjectCache()
    {
    }

}// namespace Wl