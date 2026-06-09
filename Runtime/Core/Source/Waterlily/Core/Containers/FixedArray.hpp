#pragma once

#include "Waterlily/Core/Logging/Trace.hpp"

namespace Wl
{

    template<typename ElementType, size_t Capacity>
    class FixedArray
    {
    public:
        using value_type = ElementType;
        using size_type = size_t;
        using reference = ElementType&;
        using const_reference = const ElementType&;
        using iterator = ElementType*;
        using const_iterator = const ElementType*;

    public:
        inline constexpr iterator begin()
        {
            return m_data;
        }

        inline constexpr const_iterator begin() const
        {
            return m_data;
        }

        inline constexpr const_iterator cbegin() const
        {
            return m_data;
        }

        inline constexpr iterator end()
        {
            return m_data + Capacity;
        }

        inline constexpr const_iterator end() const
        {
            return m_data + Capacity;
        }

        inline constexpr const_iterator cend() const
        {
            return m_data + Capacity;
        }

        inline constexpr size_t GetSizeInBytes() const
        {
            return GetSize() * sizeof(value_type);
        }

        inline constexpr size_t GetSize() const
        {
            return Capacity;
        }

        inline constexpr size_t GetCapacity() const
        {
            return Capacity;
        }

        inline constexpr bool IsEmpty() const
        {
            return GetSize() == 0;
        }

        inline constexpr const_reference operator[](size_t index) const
        {
            WL_CHECK_MSG(index < Capacity, "Index out of bounds.");
            return m_data[index];
        }

        inline reference operator[](size_t index)
        {
            WL_CHECK_MSG(index < Capacity, "Index out of bounds.");
            return m_data[index];
        }

        inline const value_type* GetData() const
        {
            return m_data;
        }

        inline value_type* GetData()
        {
            return m_data;
        }

    public:
        constexpr FixedArray() = default;

        constexpr FixedArray(std::initializer_list<ElementType> init)
        {
            WL_CHECK_MSG(init.size() <= Capacity, "Initializer list size exceeds static array capacity.");
            size_t i = 0;
            for (const ElementType& item: init)
            {
                WL_PLACEMENT_NEW(m_data + i++)
                ElementType(item);
            }
        }

        constexpr FixedArray(const FixedArray& other)
        {
            for (size_t i = 0; i < Capacity; i++)
            {
                WL_PLACEMENT_NEW(m_data + i)
                ElementType(other.m_data[i]);
            }
        }

        constexpr FixedArray(FixedArray&& other) noexcept
        {
            for (size_t i = 0; i < Capacity; i++)
            {
                WL_PLACEMENT_NEW(m_data + i)
                ElementType(std::move(other.m_data[i]));
            }
        }

        constexpr FixedArray& operator=(const FixedArray& other)
        {
            if (this != &other)
            {
                for (size_t i = 0; i < Capacity; i++)
                {
                    m_data[i].~ElementType();
                    WL_PLACEMENT_NEW(m_data + i)
                    ElementType(other.m_data[i]);
                }
            }
            return *this;
        }

        constexpr FixedArray& operator=(FixedArray&& other) noexcept
        {
            if (this != &other)
            {
                for (size_t i = 0; i < Capacity; i++)
                {
                    m_data[i].~ElementType();
                    WL_PLACEMENT_NEW(m_data + i)
                    ElementType(std::move(other.m_data[i]));
                }
            }
            return *this;
        }

        ~FixedArray() = default;

    private:
        ElementType m_data[Capacity];
    };

}// namespace Wl