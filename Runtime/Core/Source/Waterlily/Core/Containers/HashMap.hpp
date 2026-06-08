#pragma once

#include "Waterlily/Core/Containers/Array.hpp"
#include "Waterlily/Core/Containers/Entry.hpp"
#include "Waterlily/Core/Containers/HashMapSlot.hpp"
#include "Waterlily/Core/Defines.hpp"
#include "Waterlily/Core/Hash/Hasher.hpp"
#include "Waterlily/Core/Memory/Concepts.hpp"
#include "Waterlily/Core/Memory/DefaultAllocator.hpp"

#include <cstddef>
#include <type_traits>

namespace Wl
{

    template<typename KeyType,
             typename ValueType,
             typename HashType = Hash<KeyType>,
             CAlignedAllocator AllocatorType = DefaultAllocator>
    class HashMap
    {
    public:
        constexpr static double s_LoadFactor = 0.75;
        static_assert(s_LoadFactor > 0.0 && s_LoadFactor < 1.0, "Load factor must be between 0.0 and 1.0");

    public:
        using EntryType = Entry<const KeyType&, ValueType&>;
        using ImmutableEntryType = Entry<const KeyType&, const ValueType&>;
        using SlotType = BufferedSlot<KeyType, ValueType>;
        using SlotArray = Array<SlotType, AllocatorType>;

    public:
        using size_type = size_t;
        using value_type = EntryType;
        using pointer = EntryType*;
        using reference = EntryType&;

    private:
        class EntryIterator
        {
        public:
            using iterator_category = std::forward_iterator_tag;
            using difference_type = std::ptrdiff_t;
            using pointer = EntryType*;
            using reference = EntryType&;
            using value_type = EntryType;

        public:
            EntryIterator(HashMap* table, size_type index)
                : m_table(table)
                , m_index(index)
            {
                Next();
            }

            EntryIterator& operator++()
            {
                m_index++;
                Next();
                return *this;
            }

            EntryIterator operator++(int32_t)
            {
                EntryIterator tmp = *this;
                ++(*this);
                return tmp;
            }

            bool operator==(const EntryIterator& other) const
            {
                return m_table == other.m_table && m_index == other.m_index;
            }

            bool operator!=(const EntryIterator& other) const
            {
                return m_table != other.m_table || m_index != other.m_index;
            }

            value_type operator*() const
            {
                SlotType& slot = m_table->m_slots[m_index];
                return {slot.Key.GetRef(), slot.Value.GetRef()};
            }


        private:
            void Next()
            {
                while (m_index < m_table->GetCapacity() && !m_table->m_slots[m_index].IsOccupied())
                {
                    m_index++;
                }
            }

        private:
            HashMap* m_table;
            size_type m_index;
        };

        class ImmutableEntryIterator
        {
        public:
            using iterator_category = std::forward_iterator_tag;
            using difference_type = std::ptrdiff_t;
            using value_type = ImmutableEntryType;
            using pointer = ImmutableEntryType*;
            using reference = ImmutableEntryType&;

        public:
            ImmutableEntryIterator(const HashMap* table, size_type index)
                : m_table(table)
                , m_index(index)
            {
                Next();
            }

            ImmutableEntryIterator& operator++()
            {
                m_index++;
                Next();
                return *this;
            }

            ImmutableEntryIterator operator++(int32_t)
            {
                ImmutableEntryIterator tmp = *this;
                ++(*this);
                return tmp;
            }

            bool operator==(const ImmutableEntryIterator& other) const
            {
                return m_table == other.m_table && m_index == other.m_index;
            }

            bool operator!=(const ImmutableEntryIterator& other) const
            {
                return m_table != other.m_table || m_index != other.m_index;
            }

            value_type operator*() const
            {
                const SlotType& slot = m_table->m_slots[m_index];
                return ImmutableEntryType{slot.Key.GetRef(), slot.Value.GetRef()};
            }

        private:
            void Next()
            {
                while (m_index < m_table->GetCapacity() && !m_table->m_slots[m_index].IsOccupied())
                {
                    m_index++;
                }
            }

        private:
            const HashMap* m_table;
            size_type m_index;
        };

    public:
        using iterator = EntryIterator;
        using const_iterator = ImmutableEntryIterator;

    public:
        HashMap(size_type capacity, AllocatorType allocator = {}) noexcept
            : m_slots(allocator)
            , m_size(0)
        {
            m_slots.Resize(capacity);
        }

        HashMap() noexcept
            : m_size(0)
            , m_slots(16, AllocatorType())
        {
        }

        HashMap(std::initializer_list<ImmutableEntryType> init) noexcept
            : m_size(0)
        {
            for (const ImmutableEntryType& entry: init)
            {
                Insert(entry);
            }
        }

        HashMap(const HashMap& other) noexcept
            : m_hasher(other.m_hasher)
            , m_size(other.m_size)
            , m_slots(other.m_slots)
        {
        }

        HashMap(HashMap&& other) noexcept
            : m_hasher(std::move(other.m_hasher))
            , m_size(other.m_size)
            , m_slots(std::move(other.m_slots))
        {
        }

        HashMap& operator=(const HashMap& other) noexcept
        {
            if (this == &other)
            {
                return *this;
            }

            m_size = other.m_size;
            m_hasher = other.m_hasher;
            m_slots = other.m_slots;

            return *this;
        }

        HashMap& operator=(HashMap&& other) noexcept
        {
            if (this == &other)
            {
                return *this;
            }

            m_size = other.m_size;
            m_hasher = std::move(other.m_hasher);
            m_slots = std::move(other.m_slots);

            return *this;
        }

        ~HashMap() noexcept
        {
        }

        ValueType& operator[](const KeyType& key)
        {
            SlotType& slot = LookupDefaultImpl(key);
            return slot.Value.GetRef();
        }

        ValueType& Get(const KeyType& key)
        {
            SlotType* slot = LookupImpl(key).first;
            WL_CHECK(slot);
            return slot->Value.GetRef();
        }

        const ValueType& Get(const KeyType& key) const
        {
            const SlotType* slot = LookupImpl(key).first;
            WL_CHECK(slot);
            return slot->Value.GetRef();
        }

        ValueType* GetPtr(const KeyType& key)
        {
            if (SlotType* slot = LookupImpl(key).first)
            {
                return slot->Value.GetPtr();
            }
            return nullptr;
        }

        const ValueType* GetPtr(const KeyType& key) const
        {
            if (const SlotType* slot = LookupImpl(key).first)
            {
                return slot->Value.GetPtr();
            }
            return nullptr;
        }

        EntryType Put(const KeyType& key, const ValueType& value)
        {
            return Insert(ImmutableEntryType{key, value});
        }

        EntryType Emplace(const KeyType& key, const ValueType& value)
        {
            return Insert(ImmutableEntryType{key, value});
        }

        EntryType Insert(const ImmutableEntryType& entry)
        {
            SlotType& slot = InsertImpl(entry);
            return EntryType{slot.Key.GetRef(), slot.Value.GetRef()};
        }

        EntryType Insert(const EntryType& entry)
        {
            SlotType& slot = InsertImpl(ImmutableEntryType(entry.Key, entry.value));
            return EntryType{slot.Key.GetRef(), slot.Value.GetRef()};
        }

        bool Remove(const KeyType& key)
        {
            return RemoveImpl(key);
        }

        bool Contains(const KeyType& key) const
        {
            return LookupImpl(key).first != nullptr;
        }

        bool IsEmpty() const
        {
            return m_size == 0;
        }

        size_type GetCapacity() const
        {
            return m_slots.GetSize();
        }

        size_type GetSize() const
        {
            return m_size;
        }

        void Clear()
        {
            for (size_type i = 0; i < GetCapacity(); i++)
            {
                m_slots[i].Destroy();
            }
            m_slots.Clear();
            m_size = 0;
        }

        iterator Find(const KeyType& key)
        {
            auto [slot, index] = LookupImpl(key);
            if (slot)
            {
                return iterator(this, index);
            }
            return end();
        }

        const_iterator Find(const KeyType& key) const
        {
            auto [slot, index] = LookupImpl(key);
            if (slot)
            {
                return const_iterator(this, index);
            }
            return cend();
        }

        iterator find(const KeyType& key)
        {
            return Find(key);
        }

        const_iterator find(const KeyType& key) const
        {
            return Find(key);
        }

        const_iterator begin() const
        {
            return const_iterator(this, 0);
        }

        const_iterator end() const
        {
            return const_iterator(this, GetCapacity());
        }

        iterator begin()
        {
            return iterator(this, 0);
        }

        iterator end()
        {
            return iterator(this, GetCapacity());
        }

        const_iterator cbegin() const
        {
            return const_iterator(this, 0);
        }

        const_iterator cend() const
        {
            return const_iterator(this, GetCapacity());
        }

    private:
        SlotType& InsertImpl(const ImmutableEntryType& entry)
        {
            EnsureCapacity();

            size_type index = IndexOf(entry.Key);
            size_type start = index;

            while (m_slots[index].IsOccupied())
            {
                SlotType& slot = m_slots[index];
                if (slot.Key.GetRef() == entry.Key)
                {
                    slot.EmplaceValue(entry.Value);
                    return slot;
                }

                index = CurrentProbing(index);

                if (start == index)
                {
                    break;
                }
            }

            SlotType& slot = m_slots[index];
            slot.Emplace(entry.Key, entry.Value);
            m_size++;
            return m_slots[index];
        }

        std::pair<const SlotType*, size_type> LookupImpl(const KeyType& key) const
        {
            if (IsEmpty())
            {
                return {nullptr, m_size};
            }

            size_type index = IndexOf(key);
            size_type start = index;

            while (!m_slots[index].IsEmpty())
            {
                const SlotType& slot = m_slots[index];
                if (slot.IsOccupied() && slot.Key.GetRef() == key)
                {
                    return {&slot, index};
                }

                index = CurrentProbing(index);

                if (index == start)
                {
                    return {nullptr, index};
                }
            }

            return {nullptr, index};
        }

        std::pair<SlotType*, size_type> LookupImpl(const KeyType& key)
        {
            if (IsEmpty())
            {
                return {nullptr, m_size};
            }

            size_type index = IndexOf(key);
            size_type start = index;

            while (!m_slots[index].IsEmpty())
            {
                SlotType& slot = m_slots[index];
                if (slot.IsOccupied() && slot.Key.GetRef() == key)
                {
                    return {&slot, index};
                }

                index = CurrentProbing(index);

                if (index == start)
                {
                    return {nullptr, index};
                }
            }

            return {nullptr, index};
        }

        SlotType& LookupDefaultImpl(const KeyType& key)
        {
            EnsureCapacity();

            size_type index = IndexOf(key);
            size_type start = index;

            size_type invalidIndexRemoved = UINT64_MAX;
            size_type indexRemoved = invalidIndexRemoved;

            while (!m_slots[index].IsEmpty())
            {
                SlotType& slot = m_slots[index];
                if (slot.IsOccupied() && slot.Key.GetRef() == key)
                {
                    return slot;
                }

                if (slot.IsRemoved() && indexRemoved == invalidIndexRemoved)
                {
                    indexRemoved = index;
                }

                index = CurrentProbing(index);

                if (index == start)
                {
                    break;
                }
            }

            index = (indexRemoved != invalidIndexRemoved) ? indexRemoved : index;

            SlotType& slot = m_slots[index];
            slot.EmplaceKey(key);

            if constexpr (std::is_default_constructible_v<ValueType>)
            {
                slot.EmplaceValue(ValueType());
            }

            slot.State = SlotState::Occupied;
            m_size++;
            return slot;
        }

        bool RemoveImpl(const KeyType& key)
        {
            if (IsEmpty())
            {
                return false;
            }

            size_type index = IndexOf(key);
            size_type start = index;

            while (!m_slots[index].IsEmpty())
            {
                SlotType& slot = m_slots[index];
                if (slot.IsOccupied() && slot.Key.GetRef() == key)
                {
                    slot.Destroy();
                    m_size--;
                    return true;
                }

                index = CurrentProbing(index);

                if (index == start)
                {
                    return false;
                }
            }

            return false;
        }

        void Rehash(size_type newCapacity)
        {
            SlotArray oldSlots = std::move(m_slots);

            m_size = 0;

            size_type capacity = newCapacity;
            m_slots = SlotArray();
            m_slots.Resize(capacity);

            for (size_type i = 0; i < capacity; i++)
            {
                m_slots[i].State = SlotState::Empty;
            }

            for (size_type i = 0; i < oldSlots.GetSize(); i++)
            {
                SlotType& oldSlot = oldSlots[i];
                if (oldSlot.IsOccupied())
                {
                    Insert(ImmutableEntryType{oldSlot.Key.GetRef(), oldSlot.Value.GetRef()});
                }
            }
        }

        void EnsureCapacity()
        {
            if ((m_size + 1) > GetCapacity() * s_LoadFactor)
            {
                size_t newCapacity = GetCapacity() == 0 ? 16 : GetCapacity() * 2;
                Rehash(newCapacity);
            }
        }

        size_type CurrentProbing(size_type index) const
        {
            return LinearProbing(index);
        }

        size_type LinearProbing(size_type index) const
        {
            return (index + 1) % GetCapacity();
        }

        size_type QuadraticProbing(size_type index) const
        {
            return (index + 1) * (index + 1) % GetCapacity();
        }

        size_type IndexOf(const KeyType& key) const
        {
            return m_hasher(key) % GetCapacity();
        }

    private:
        HashType m_hasher;
        SlotArray m_slots;
        size_type m_size;
    };

}// namespace Wl