#pragma once

#include "Waterlily/Core/Hash/Hasher.hpp"
#include "Waterlily/Core/Logging/Trace.hpp"
#include "Waterlily/Core/Memory/DefaultAllocator.hpp"
#include "Waterlily/Core/Memory/Memory.hpp"

#include <initializer_list>
#include <utility>

namespace Wl
{

    template<typename KeyType>
    struct HashSetElement
    {
        KeyType Key;
        HashSetElement<KeyType>* Next = nullptr;
        size_t Hash = 0;

        HashSetElement() = default;

        HashSetElement(const KeyType& key, size_t hash)
            : Key(key)
            , Next(nullptr)
            , Hash(hash)
        {
        }

        HashSetElement(KeyType&& key, size_t hash)
            : Key(std::move(key))
            , Next(nullptr)
            , Hash(hash)
        {
        }
    };

    template<typename KeyType,
             CTypedAllocator<HashSetElement<KeyType>> AllocatorType = DefaultTypedAllocator<HashSetElement<KeyType>>,
             typename HasherType = Hasher>
    class HashSet
    {
    public:
        static constexpr double s_LoadFactor = 0.75;

        using ElementType = HashSetElement<KeyType>;
        using value_type = KeyType;

        class Iterator
        {
        public:
            Iterator(HashSet* set = nullptr, size_t bucket_index = 0, ElementType* elem = nullptr)
                : m_set(set)
                , m_bucket_index(bucket_index)
                , m_elem(elem)
            {
            }

            value_type& operator*() const
            {
                WL_CHECK_MSG(m_elem, "Iterator de-referencing null element");
                return m_elem->Key;
            }

            value_type* operator->() const
            {
                return &m_elem->Key;
            }

            Iterator& operator++()
            {
                if (!m_set || !m_elem)
                {
                    m_bucket_index = m_set ? m_set->m_capacity : 0;
                    m_elem = nullptr;
                    return *this;
                }

                if (m_elem->Next)
                {
                    m_elem = m_elem->Next;
                    return *this;
                }

                m_bucket_index++;
                while (m_bucket_index < m_set->m_capacity)
                {
                    if (m_set->m_buckets[m_bucket_index])
                    {
                        m_elem = m_set->m_buckets[m_bucket_index];
                        return *this;
                    }
                    ++m_bucket_index;
                }

                // Fin
                m_elem = nullptr;
                return *this;
            }

            Iterator operator++(int)
            {
                Iterator tmp = *this;
                ++(*this);
                return tmp;
            }

            bool operator==(const Iterator& other) const
            {
                return m_set == other.m_set && m_bucket_index == other.m_bucket_index && m_elem == other.m_elem;
            }

            bool operator!=(const Iterator& other) const
            {
                return !(*this == other);
            }

        private:
            HashSet* m_set;
            size_t m_bucket_index;
            ElementType* m_elem;
        };

        class ConstIterator
        {
        public:
            ConstIterator(const HashSet* set = nullptr, size_t bucket_index = 0, const ElementType* elem = nullptr)
                : m_set(set)
                , m_bucket_index(bucket_index)
                , m_elem(elem)
            {
            }

            const value_type& operator*() const
            {
                return m_elem->Key;
            }

            const value_type* operator->() const
            {
                return &m_elem->Key;
            }

            ConstIterator& operator++()
            {
                if (!m_set || !m_elem)
                {
                    m_bucket_index = m_set ? m_set->m_capacity : 0;
                    m_elem = nullptr;
                    return *this;
                }

                if (m_elem->Next)
                {
                    m_elem = m_elem->Next;
                    return *this;
                }

                ++m_bucket_index;
                while (m_bucket_index < m_set->m_capacity)
                {
                    if (m_set->m_buckets[m_bucket_index])
                    {
                        m_elem = m_set->m_buckets[m_bucket_index];
                        return *this;
                    }
                    ++m_bucket_index;
                }

                m_elem = nullptr;
                return *this;
            }

            ConstIterator operator++(int)
            {
                ConstIterator tmp = *this;
                ++(*this);
                return tmp;
            }

            bool operator==(const ConstIterator& other) const
            {
                return m_set == other.m_set && m_bucket_index == other.m_bucket_index && m_elem == other.m_elem;
            }

            bool operator!=(const ConstIterator& other) const
            {
                return !(*this == other);
            }

        private:
            const HashSet* m_set;
            size_t m_bucket_index;
            const ElementType* m_elem;
        };

        using iterator = Iterator;
        using const_iterator = ConstIterator;

    public:
        HashSet(size_t capacity = 8)
            : m_size(0)
            , m_capacity(capacity)
            , m_buckets(nullptr)
            , m_allocator()
        {
            InitializeBuckets();
        }

        HashSet(std::initializer_list<KeyType> init)
            : m_size(0)
            , m_capacity(8)
            , m_buckets(nullptr)
            , m_allocator()
        {
            InitializeBuckets();
            for (const auto& key: init)
            {
                Insert(key);
            }
        }

        HashSet(const HashSet& other)
            : m_size(0)
            , m_capacity(other.m_capacity)
            , m_buckets(nullptr)
            , m_allocator()
        {
            InitializeBuckets();

            for (size_t i = 0; i < other.m_capacity; i++)
            {
                ElementType* cur = other.m_buckets[i];
                ElementType** tail = &m_buckets[i];
                while (cur)
                {
                    ElementType* copy = m_allocator.Allocate(1);
                    WL_PLACEMENT_NEW(copy)
                    ElementType(cur->Key, cur->Hash);
                    *tail = copy;
                    tail = &((*tail)->Next);
                    cur = cur->Next;
                    ++m_size;
                }
            }
        }

        HashSet(HashSet&& other) noexcept
            : m_size(other.m_size)
            , m_capacity(other.m_capacity)
            , m_buckets(other.m_buckets)
            , m_allocator(std::move(other.m_allocator))
        {
            other.m_size = 0;
            other.m_capacity = 0;
            other.m_buckets = nullptr;
        }

        ~HashSet()
        {
            Clear();
            // FIXME: use Memory::Deallocate.
            delete[] m_buckets;
            m_buckets = nullptr;
        }

        HashSet& operator=(const HashSet& other)
        {
            if (this == &other)
            {
                return *this;
            }

            HashSet tmp(other);
            Swap(tmp);
            return *this;
        }

        HashSet& operator=(HashSet&& other) noexcept
        {
            if (this == &other)
            {
                return *this;
            }

            Clear();
            // TODO: use ldelete_array.
            delete[] m_buckets;

            m_allocator = std::move(other.m_allocator);
            m_size = other.m_size;
            m_capacity = other.m_capacity;
            m_buckets = other.m_buckets;
            other.m_size = 0;
            other.m_capacity = 0;
            other.m_buckets = nullptr;

            return *this;
        }

        iterator Insert(const KeyType& key)
        {
            if (!m_buckets)
            {
                InitializeBuckets();
            }

            if (static_cast<double>(m_size + 1) > static_cast<double>(m_capacity) * s_LoadFactor)
            {
                Resize(m_capacity * 2);
            }

            size_t h = Hash(key);
            size_t idx = static_cast<size_t>(h % m_capacity);

            ElementType* cur = m_buckets[idx];
            while (cur)
            {
                if (cur->Key == key)
                {
                    return iterator(this, idx, cur);
                }
                cur = cur->Next;
            }

            ElementType* ne = CreateElement(key, h);
            ne->Next = m_buckets[idx];
            m_buckets[idx] = ne;
            m_size++;
            return Iterator(this, idx, ne);
        }

        KeyType& Add(const KeyType& key)
        {
            return *Insert(key);
        }

        KeyType& Add(KeyType&& key)
        {
            return *Insert(key);
        }

        iterator Insert(KeyType&& key)
        {
            if (!m_buckets)
            {
                InitializeBuckets();
            }

            if (static_cast<double>(m_size + 1) > static_cast<double>(m_capacity) * s_LoadFactor)
            {
                Resize(m_capacity * 2);
            }

            size_t h = Hash(key);
            size_t idx = static_cast<size_t>(h % m_capacity);

            ElementType* cur = m_buckets[idx];
            while (cur)
            {
                if (cur->Key == key)
                {
                    return iterator(this, idx, cur);
                }
                cur = cur->Next;
            }

            ElementType* ne = CreateElement(std::move(key), h);
            ne->Next = m_buckets[idx];
            m_buckets[idx] = ne;
            m_size++;
            return iterator(this, idx, ne);
        }

        size_t Remove(const KeyType& key)
        {
            if (!m_buckets)
            {
                return 0;
            }

            size_t h = Hash(key);
            size_t idx = static_cast<size_t>(h % m_capacity);

            ElementType* cur = m_buckets[idx];
            ElementType* prev = nullptr;

            while (cur)
            {

                if (cur->Key == key)
                {

                    if (prev)
                    {
                        prev->Next = cur->Next;
                    }
                    else
                    {
                        m_buckets[idx] = cur->Next;
                    }

                    cur->~ElementType();
                    m_allocator.Deallocate(cur, 1);
                    --m_size;

                    return 1;
                }

                prev = cur;
                cur = cur->Next;
            }

            return 0;
        }

        iterator Find(const KeyType& key)
        {
            if (!m_buckets)
            {
                return end();
            }

            size_t h = Hash(key);
            size_t idx = static_cast<size_t>(h % m_capacity);
            ElementType* cur = m_buckets[idx];

            while (cur)
            {
                if (cur->Key == key)
                {
                    return iterator(this, idx, cur);
                }
                cur = cur->Next;
            }

            return end();
        }

        const_iterator Find(const KeyType& key) const
        {
            if (!m_buckets)
            {
                return end();
            }

            size_t h = Hash(key);
            size_t idx = static_cast<size_t>(h % m_capacity);
            ElementType* cur = m_buckets[idx];

            while (cur)
            {
                if (cur->Key == key)
                {
                    return const_iterator(this, idx, cur);
                }
                cur = cur->Next;
            }

            return end();
        }

        iterator find(const KeyType& key)
        {
            return Find(key);
        }

        const_iterator find(const KeyType& key) const
        {
            return Find(key);
        }

        bool Contains(const KeyType& key) const
        {
            return lookup_impl(key) != nullptr;
        }

        void Clear()
        {
            if (!m_buckets)
            {
                return;
            }

            for (size_t i = 0; i < m_capacity; i++)
            {
                ElementType* cur = m_buckets[i];

                while (cur)
                {
                    ElementType* next = cur->Next;
                    cur->~ElementType();
                    m_allocator.Deallocate(cur, 1);
                    cur = next;
                }

                m_buckets[i] = nullptr;
            }

            m_size = 0;
        }

        bool IsEmpty() const
        {
            return m_size == 0;
        }

        size_t GetSize() const
        {
            return m_size;
        }

        size_t GetCapacity() const
        {
            return m_capacity;
        }

        iterator begin()
        {
            if (!m_buckets)
            {
                return end();
            }

            for (size_t i = 0; i < m_capacity; i++)
            {
                if (m_buckets[i])
                {
                    return iterator(this, i, m_buckets[i]);
                }
            }

            return end();
        }

        iterator end()
        {
            return iterator(this, m_capacity, nullptr);
        }

        const_iterator begin() const
        {
            if (!m_buckets)
            {
                return end();
            }

            for (size_t i = 0; i < m_capacity; i++)
            {
                if (m_buckets[i])
                {
                    return const_iterator(this, i, m_buckets[i]);
                }
            }

            return end();
        }

        const_iterator end() const
        {
            return const_iterator(this, m_capacity, nullptr);
        }

        const_iterator cbegin() const
        {
            return begin();
        }

        const_iterator cend() const
        {
            return end();
        }

        void Rehash()
        {
            Resize(m_capacity);
        }

        void Resize(size_t new_capacity)
        {
            if (new_capacity == 0)
            {
                return;
            }
            // TODO: lnew_array
            ElementType** new_buckets = new ElementType*[new_capacity];
            Memory::Write(new_buckets, 0, sizeof(ElementType*) * new_capacity);

            for (size_t i = 0; i < m_capacity; i++)
            {
                ElementType* cur = m_buckets[i];
                while (cur)
                {
                    ElementType* next = cur->Next;
                    size_t idx = static_cast<size_t>(cur->Hash % new_capacity);

                    cur->Next = new_buckets[idx];
                    new_buckets[idx] = cur;

                    cur = next;
                }
            }

            delete[] m_buckets;// TODO: ldelete_array
            m_buckets = new_buckets;
            m_capacity = new_capacity;
        }

    private:
        ElementType* lookup_impl(const KeyType& key) const
        {
            if (!m_buckets)
            {
                return nullptr;
            }
            size_t h = Hash(key);
            size_t idx = static_cast<size_t>(h % m_capacity);
            ElementType* cur = m_buckets[idx];
            while (cur)
            {
                if (cur->Key == key)
                {
                    return cur;
                }
                cur = cur->Next;
            }
            return nullptr;
        }

        ElementType* CreateElement(const KeyType& key, size_t h)
        {
            ElementType* raw = m_allocator.Allocate(1);
            ElementType* element = WL_PLACEMENT_NEW(raw) ElementType(key, h);
            return element;
        }

        ElementType* CreateElement(KeyType&& key, size_t h)
        {
            ElementType* raw = m_allocator.Allocate(1);
            ElementType* element = WL_PLACEMENT_NEW(raw) ElementType(std::move(key), h);
            return element;
        }

        size_t Hash(const KeyType& key) const
        {
            return std::hash<KeyType>{}(key);
        }

        void InitializeBuckets()
        {
            if (m_capacity == 0)
            {
                m_capacity = 1;
            }
            // TODO: Memory::Allocate
            m_buckets = new ElementType*[m_capacity];
            Memory::Write(m_buckets, 0, sizeof(ElementType*) * m_capacity);
        }

        void Swap(HashSet& other) noexcept
        {
            std::swap(m_buckets, other.m_buckets);
            std::swap(m_size, other.m_size);
            std::swap(m_capacity, other.m_capacity);
            std::swap(m_allocator, other.m_allocator);
        }

    private:
        ElementType** m_buckets;
        AllocatorType m_allocator;
        size_t m_size = 0;
        size_t m_capacity = 1;
    };

}// namespace Wl
