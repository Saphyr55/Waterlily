#pragma once

#include "Waterlily/Core/Defines.hpp"
#include "Waterlily/Core/Math/Math.hpp"
#include "Waterlily/Core/Memory/Concepts.hpp"
#include "Waterlily/Core/Memory/DefaultAllocator.hpp"
#include "Waterlily/Core/Memory/Memory.hpp"
#include "Waterlily/Core/Memory/TypedAllocator.hpp"

#include <algorithm>
#include <memory>
#include <type_traits>

namespace Wl
{

    template<typename ElementType, CAlignedAllocator AllocatorType = DefaultAllocator>
    class Array
    {
    public:
        using value_type = ElementType;
        using size_type = size_t;
        using reference = ElementType&;
        using pointer = ElementType*;
        using const_pointer = const ElementType*;
        using const_reference = const ElementType&;
        using iterator = ElementType*;
        using const_iterator = const ElementType*;

    public:
        const_reference Front() const
        {
            WL_CHECK_MSG(!IsEmpty(), "Call front with an empty array.");
            return m_data[0];
        }

        reference Front()
        {
            WL_CHECK_MSG(!IsEmpty(), "Call front with an empty array.");
            return m_data[0];
        }

        const_reference Back() const
        {
            WL_CHECK_MSG(!IsEmpty(), "Call back with an empty array.");
            return m_data[m_size - 1];
        }

        reference Back()
        {
            WL_CHECK_MSG(!IsEmpty(), "Call back with an empty array.");
            return m_data[m_size - 1];
        }

        void Append(const ElementType& element)
        {
            if (!m_data)
            {
                Reallocate(m_capacity);
            }

            if (m_size >= m_capacity)
            {
                Reserve(m_capacity == 0 ? 1 : m_capacity * s_GrowFactor);
            }

            new (m_data + m_size) ElementType(element);

            m_size++;
        }

        void Append(ElementType&& element)
        {
            if (!m_data)
            {
                Reallocate(m_capacity);
            }

            if (m_size >= m_capacity)
            {
                Reserve(m_capacity == 0 ? 1 : m_capacity * s_GrowFactor);
            }

            WL_PLACEMENT_NEW(m_data + m_size)
            ElementType(std::move(element));
            m_size++;
        }

        void AppendRange(iterator first, iterator last)
        {
            size_type count = static_cast<size_type>(std::distance(first, last));

            if (count == 0)
            {
                return;
            }

            if (!m_data)
            {
                Reallocate(m_capacity);
            }

            size_type required = m_size + count;

            if (required > m_capacity)
            {
                Reserve(Math::Max(required, m_capacity * s_GrowFactor));
            }

            if constexpr (std::is_trivially_copyable_v<ElementType>)
            {
                Memory::Copy(m_data + m_size, &(*first), sizeof(ElementType) * count);
                m_size += count;
            }
            else
            {
                for (iterator it = first; it != last; it++)
                {
                    WL_PLACEMENT_NEW(m_data + m_size)
                    ElementType(*it);
                    m_size++;
                }
            }
        }

        template<typename OtherAllocatorType>
        void AppendRange(const Array<ElementType, OtherAllocatorType>& array)
        {
            AppendRange(array.begin(), array.end());
        }

        template<typename... Args>
        reference Emplace(Args&&... args)
        {
            if (!m_data)
            {
                Reallocate(m_capacity);
            }

            if (m_size >= m_capacity)
            {
                Reserve(m_capacity == 0 ? 1 : m_capacity * s_GrowFactor);
            }

            WL_PLACEMENT_NEW(m_data + m_size)
            ElementType(std::forward<Args>(args)...);
            m_size++;
            return Back();
        }

        void Pop()
        {
            if (m_size > 0)
            {
                m_size--;
                m_data[m_size].~ElementType();
            }
        }

        value_type PopBack()
        {
            value_type value = Back();
            Pop();
            return value;
        }

        inline ElementType& operator[](size_type index)
        {
            WL_CHECK_MSG(index < m_size, "Index out of bounds.");
            return m_data[index];
        }

        inline const ElementType& operator[](size_type index) const
        {
            WL_CHECK_MSG(index < m_size, "Index out of bounds.");
            return m_data[index];
        }

        inline size_type GetSize() const
        {
            return m_size;
        }

        inline size_t GetSizeInBytes() const
        {
            return m_size * sizeof(ElementType);
        }

        inline size_type GetCapacity() const
        {
            return m_capacity;
        }

        inline bool IsEmpty() const
        {
            return GetSize() == 0;
        }

        void Clear()
        {
            if constexpr (std::is_destructible_v<ElementType>)
            {
                for (size_type i = 0; i < m_size; i++)
                {
                    m_data[i].~ElementType();
                }
            }
            AllocatorDeallocate(m_data, m_size);
            m_capacity = 2;
            m_data = AllocatorAllocate(m_capacity);
            m_size = 0;
        }

        void RemoveIf(auto&& predicate)
        {
            size_type newSize = 0;

            for (size_type i = 0; i < m_size; i++)
            {
                if (!predicate(m_data[i]))
                {
                    if (newSize != i)
                    {
                        m_data[newSize] = std::move(m_data[i]);
                    }
                    newSize++;
                }
                else
                {
                    m_data[i].~ElementType();
                }
            }

            m_size = newSize;
        }

        void Remove(const ElementType& value)
        {
            RemoveIf([&value](const ElementType& elem) -> bool
            {
                return (elem == value);
            });
        }

        void Resize(size_type size, const ElementType& defaultElement = ElementType())
        {
            if (size > m_capacity)
            {
                Reserve(size);
            }
            else if (!m_data && m_capacity > 0)
            {
                m_data = AllocatorAllocate(m_capacity);
            }

            if (size > m_size)
            {
                for (size_type i = m_size; i < size; i++)
                {
                    WL_PLACEMENT_NEW(m_data + i)
                    ElementType(defaultElement);
                }
            }
            else
            {
                if constexpr (std::is_destructible_v<ElementType>)
                {
                    for (size_type i = size; i < m_size; i++)
                    {
                        m_data[i].~ElementType();
                    }
                }
            }

            m_size = size;
        }

        void Reserve(size_type capacity)
        {
            if (capacity > m_capacity)
            {
                Reallocate(capacity);
            }
        }

        void Shrink()
        {
            if (m_size < m_capacity)
            {
                Reallocate(m_size);
            }
        }

        void Swap(Array& other) noexcept
        {
            std::swap(m_data, other.m_data);
            std::swap(m_size, other.m_size);
            std::swap(m_capacity, other.m_capacity);
            std::swap(m_allocator, other.m_allocator);
        }

        constexpr const ElementType* GetData() const
        {
            return m_data;
        }

        constexpr ElementType* GetData()
        {
            return m_data;
        }

        bool Contains(const ElementType& element) const
        {
            for (size_type i = 0; i < m_size; i++)
            {
                if (m_data[i] == element)
                {
                    return true;
                }
            }
            return false;
        }

        bool Contains(const ElementType& element, auto&& predicate) const
        {
            for (size_type i = 0; i < m_size; i++)
            {
                if (predicate(m_data[i], element))
                {
                    return true;
                }
            }
            return false;
        }

        template<typename Predicate>
        ElementType* GetIf(Predicate&& predicate) const
        {
            for (size_type i = 0; i < m_size; i++)
            {
                if (predicate(m_data[i]))
                {
                    return &m_data[i];
                }
            }
            return nullptr;
        }

        AllocatorType& GetAllocator()
        {
            return m_allocator;
        }

        const AllocatorType& GetAllocator() const
        {
            return m_allocator;
        }

    public:
        Array(const AllocatorType& allocator = {}) noexcept
            : m_data(nullptr)
            , m_size(0)
            , m_capacity(2)
            , m_allocator(allocator)
        {
        }

        Array(size_t capacity, const AllocatorType& allocator = {}) noexcept
            : m_data(nullptr)
            , m_size(0)
            , m_capacity(capacity)
            , m_allocator(allocator)
        {
            m_data = AllocatorAllocate(m_capacity);
        }

        Array(std::initializer_list<ElementType> init, const AllocatorType& allocator = {})
            : m_allocator(allocator)
            , m_data(nullptr)
            , m_size(init.size())
            , m_capacity(init.size())
        {
            if (m_capacity == 0)
            {
                m_capacity = 2;
            }
            m_data = AllocatorAllocate(m_capacity);
            size_type i = 0;
            for (const ElementType& item: init)
            {
                WL_PLACEMENT_NEW(m_data + i++)
                ElementType(item);
            }
        }

        Array(ElementType* elements, size_type size, const AllocatorType& allocator = {})
            : m_data(nullptr)
            , m_size(size)
            , m_capacity(size)
            , m_allocator(allocator)
        {
            CopyData(elements);
        }

        Array(const Array& other) noexcept
            : m_allocator(other.m_allocator)
            , m_data(nullptr)
            , m_size(other.m_size)
            , m_capacity(other.m_capacity)
        {
            CopyData(other.m_data);
        }

        Array(Array&& other) noexcept
            : m_data(other.m_data)
            , m_size(other.m_size)
            , m_capacity(other.m_capacity)
            , m_allocator(std::move(other.m_allocator))
        {
            other.m_data = nullptr;
            other.m_size = 0;
            other.m_capacity = 0;
        }

        ~Array()
        {
            if (m_data)
            {
                Clear();
                AllocatorDeallocate(m_data, m_capacity);
                m_data = nullptr;
            }
        }

        Array& operator=(const Array& other)
        {
            if (this != &other)
            {
                Array temp(other);
                Swap(temp);
            }
            return *this;
        }

        Array& operator=(Array&& other) noexcept
        {
            if (this != &other)
            {
                Clear();

                AllocatorDeallocate(m_data, m_capacity);

                m_data = other.m_data;
                m_size = other.m_size;
                m_capacity = other.m_capacity;
                m_allocator = other.m_allocator;
                other.m_data = nullptr;
                other.m_size = 0;
                other.m_capacity = 0;
            }

            return *this;
        }

    public:
        // STD compatible interface

        bool empty() const
        {
            return IsEmpty();
        }

        size_type size() const
        {
            return GetSize();
        }

        pointer data()
        {
            return GetData();
        }

        const_pointer data() const
        {
            return GetData();
        }

        const_reference front() const
        {
            WL_CHECK_MSG(!IsEmpty(), "Call front with an empty array.");
            return m_data[0];
        }

        reference front()
        {
            WL_CHECK_MSG(!IsEmpty(), "Call front with an empty array.");
            return m_data[0];
        }

        const_reference back() const
        {
            WL_CHECK_MSG(!IsEmpty(), "Call back with an empty array.");
            return m_data[m_size - 1];
        }

        reference back()
        {
            WL_CHECK_MSG(!IsEmpty(), "Call back with an empty array.");
            return m_data[m_size - 1];
        }

        void push_back(const ElementType& element)
        {
            Append(element);
        }

        void push_back(ElementType&& element)
        {
            Append(std::move(element));
        }

        iterator begin()
        {
            return m_data;
        }

        iterator begin() const
        {
            return m_data;
        }

        const_iterator cbegin() const
        {
            return m_data;
        }

        iterator end()
        {
            return m_data + m_size;
        }

        iterator end() const
        {
            return m_data + m_size;
        }

        const_iterator cend() const
        {
            return m_data + m_size;
        }

    private:
        void CopyData(ElementType* source)
        {
            if (m_size <= 0)
            {
                return;
            }

            m_data = AllocatorAllocate(m_capacity);
            for (size_type i = 0; i < m_size; i++)
            {
                WL_PLACEMENT_NEW(m_data + i)
                ElementType(source[i]);
            }
        }

        void Reallocate(size_type newCapacity)
        {
            ElementType* newData = AllocatorAllocate(newCapacity);

            size_type elementsToMove = (m_size < newCapacity) ? m_size : newCapacity;

            if constexpr (std::is_trivially_copyable_v<ElementType>)
            {
                Memory::Copy(newData, m_data, sizeof(ElementType) * elementsToMove);
            }
            else
            {
                std::uninitialized_move(m_data, m_data + elementsToMove, newData);
            }

            if constexpr (std::is_destructible_v<ElementType>)
            {
                for (size_type i = 0; i < m_size; i++)
                {
                    m_data[i].~ElementType();
                }
            }

            if (m_data)
            {
                AllocatorDeallocate(m_data, m_capacity);
            }

            m_data = newData;
            m_capacity = newCapacity;

            if (newCapacity < m_size)
            {
                m_size = newCapacity;
            }
        }

        ElementType* AllocatorAllocate(size_type n)
        {
            TypedAllocator<ElementType, AllocatorType> typedAllocator(&m_allocator);
            ElementType* elements = typedAllocator.Allocate(n);
            WL_CHECK(elements);
            return elements;
        }

        void AllocatorDeallocate(ElementType* p, size_type n)
        {
            if (p)
            {
                TypedAllocator<ElementType, AllocatorType> typedAllocator(&m_allocator);
                typedAllocator.Deallocate(p, n);
            }
        }

    private:
        ElementType* m_data;
        size_type m_size;
        size_type m_capacity;
        AllocatorType m_allocator;
        static constexpr size_type s_GrowFactor = 2;
    };

    template<typename ElementType, typename AllocatorType>
    inline bool operator==(const Array<ElementType, AllocatorType>& lhs, const Array<ElementType, AllocatorType>& rhs)
    {
        if (lhs.GetSize() != rhs.GetSize())
        {
            return false;
        }

        for (size_t i = 0; i < lhs.GetSize(); i++)
        {
            if (lhs[i] != rhs[i])
            {
                return false;
            }
        }

        return true;
    }

    template<typename ElementType, typename AllocatorType>
    inline bool operator!=(const Array<ElementType, AllocatorType>& lhs, const Array<ElementType, AllocatorType>& rhs)
    {
        return !(lhs == rhs);
    }

    template<typename ElementType, typename AllocatorType>
    inline void swap(Array<ElementType, AllocatorType>& lhs,
                     Array<ElementType, AllocatorType>& rhs) noexcept(noexcept(lhs.Swap(rhs)))
    {
        lhs.Swap(rhs);
    }

}// namespace Wl
