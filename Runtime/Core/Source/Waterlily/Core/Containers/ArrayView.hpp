#pragma once

#include "Waterlily/Core/Containers/Array.hpp"
#include "Waterlily/Core/Containers/FixedArray.hpp"
#include "Waterlily/Core/Defines.hpp"
#include "Waterlily/Core/Memory/Concepts.hpp"
#include "Waterlily/Core/Memory/DefaultAllocator.hpp"

namespace Wl
{

    template<typename ElementType>
    class ArrayView
    {
    public:
        using value_type = ElementType;
        using size_type = size_t;

        using reference = ElementType&;
        using const_reference = const ElementType&;

        using pointer = ElementType*;
        using const_pointer = const ElementType*;

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
            WL_CHECK_MSG(!IsEmpty(), "Call front with an empty array.");
            return m_data[m_size - 1];
        }

        reference Back()
        {
            WL_CHECK_MSG(!IsEmpty(), "Call front with an empty array.");
            return m_data[m_size - 1];
        }

        inline constexpr reference operator[](size_type index)
        {
            WL_CHECK_MSG(index < m_size, "Index out of bounds.");
            return m_data[index];
        }

        inline constexpr const_reference operator[](size_type index) const
        {
            WL_CHECK_MSG(index < m_size, "Index out of bounds.");
            return m_data[index];
        }

        inline constexpr size_type GetSize() const
        {
            return m_size;
        }

        inline constexpr bool IsEmpty() const
        {
            return GetSize() == 0;
        }

        inline const_pointer GetData() const
        {
            return m_data;
        }

        inline pointer GetData()
        {
            return m_data;
        }

        bool Contains(const_reference element) const
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

        bool Contains(const_reference element, auto&& predicate) const
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

        pointer GetIf(auto&& p_predicate) const
        {
            for (size_type i = 0; i < m_size; i++)
            {
                if (p_predicate(m_data[i]))
                {
                    return &m_data[i];
                }
            }
            return nullptr;
        }

        constexpr ArrayView First(size_type n) const noexcept
        {
            if (n > m_size)
            {
                n = m_size;
            }
            return ArrayView(m_data, n);
        }

        constexpr ArrayView Last(size_type n) const noexcept
        {
            if (n > m_size)
            {
                n = m_size;
            }
            return ArrayView(m_data + (m_size - n), n);
        }

        constexpr ArrayView SubView(size_type offset, size_type len) const
        {
            if (offset > m_size)
            {
                offset = m_size;
            }

            if (len > m_size - offset)
            {
                len = m_size - offset;
            }

            return ArrayView(m_data + offset, len);
        }

        constexpr ArrayView SubView(size_type offset) const noexcept
        {
            if (offset > m_size)
            {
                offset = m_size;
            }
            return ArrayView(m_data + offset, m_size - offset);
        }

        constexpr bool operator==(ArrayView other) const noexcept
        {
            if (m_size != other.m_size)
            {
                return false;
            }

            for (size_type i = 0; i < m_size; i++)
            {
                if (!(m_data[i] == other.m_data[i]))
                {
                    return false;
                }
            }

            return true;
        }

        constexpr bool operator!=(ArrayView other) const noexcept
        {
            return !(*this == other);
        }

        constexpr size_type GetSizeInBytes() const noexcept
        {
            return m_size * sizeof(value_type);
        }

    public:
        const_reference front() const
        {
            return Front();
        }

        reference front()
        {
            return Front();
        }

        const_reference back() const
        {
            return Back();
        }

        reference back()
        {
            return Back();
        }

        inline constexpr size_type size() const
        {
            return GetSize();
        }

        inline constexpr bool empty() const
        {
            return IsEmpty();
        }

        inline const_pointer data() const
        {
            return GetData();
        }

        inline pointer data()
        {
            return GetData();
        }

        iterator begin()
        {
            return m_data;
        }

        iterator begin() const
        {
            return m_data;
        }

        const_iterator cbegin()
        {
            return m_data;
        }

        const_iterator cbegin() const
        {
            return m_data;
        }

        iterator end()
        {
            return const_cast<iterator>(m_data) + m_size;
        }

        iterator end() const
        {
            return m_data + m_size;
        }

        const_iterator cend()
        {
            return m_data + m_size;
        }

        const_iterator cend() const
        {
            return m_data + m_size;
        }

    public:
        constexpr ArrayView() noexcept = default;

        template<size_t N>
        constexpr ArrayView(value_type (&data)[N]) noexcept
            : m_data(data)
            , m_size(N)
        {
        }

        constexpr ArrayView(pointer data, size_type size) noexcept
            : m_data(data)
            , m_size(size)
        {
        }

        template<CAlignedAllocator AllocatorType = DefaultAllocator>
        constexpr ArrayView(const Array<value_type, AllocatorType>& array) noexcept
            : m_data(const_cast<value_type*>(array.GetData()))
            , m_size(array.GetSize())
        {
        }

        template<CAlignedAllocator AllocatorType = DefaultAllocator>
        constexpr ArrayView(Array<value_type, AllocatorType>& array) noexcept
            : m_data(array.GetData())
            , m_size(array.GetSize())
        {
        }

        template<size_type Size>
        constexpr ArrayView(const FixedArray<value_type, Size>& array) noexcept
            : m_data(const_cast<value_type*>(array.GetData()))
            , m_size(array.GetSize())
        {
        }

        template<size_type Size>
        constexpr ArrayView(FixedArray<value_type, Size>& array) noexcept
            : m_data(array.GetData())
            , m_size(array.GetSize())
        {
        }

        constexpr ArrayView(std::initializer_list<value_type> list) noexcept
            : m_data(const_cast<value_type*>(list.begin()))
            , m_size(list.size())
        {
        }

        template<typename OtherElementType = value_type>
            requires(!std::is_const_v<OtherElementType>)
        constexpr operator ArrayView<const OtherElementType>() const noexcept
        {
            return ArrayView<const OtherElementType>(m_data, m_size);
        }

    private:
        value_type* m_data;
        size_type m_size;
    };


}// namespace Wl