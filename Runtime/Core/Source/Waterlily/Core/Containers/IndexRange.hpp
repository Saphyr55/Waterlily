#pragma once

#include "Waterlily/Core/Defines.hpp"

#include <concepts>
#include <cstddef>
#include <iterator>

namespace Wl
{

    template<std::integral IndexType = size_t>
    class IndexRange
    {
    public:
        class Iterator
        {
        public:
            using iterator_category = std::forward_iterator_tag;
            using value_type = IndexType;
            using difference_type = IndexType;
            using pointer = IndexType*;
            using reference = IndexType&;

            constexpr explicit Iterator(IndexType index)
                : m_index(index)
            {
            }

            constexpr value_type operator*() const
            {
                return m_index;
            }

            constexpr Iterator operator++()
            {
                ++m_index;
                return *this;
            }

            constexpr Iterator operator++(int)
            {
                Iterator temp = *this;
                ++m_index;
                return temp;
            }

            constexpr bool operator==(const Iterator& other) const
            {
                return m_index == other.m_index;
            }

            constexpr bool operator!=(const Iterator& other) const
            {
                return m_index != other.m_index;
            }

        private:
            IndexType m_index;
        };

        constexpr IndexRange(IndexType start, IndexType end)
            : m_start(start)
            , m_end(end)
        {
            WL_CHECK(start <= end);
        }

        constexpr IndexRange(const auto& start, const auto& end)
            : m_start(static_cast<IndexType>(start))
            , m_end(static_cast<IndexType>(end))
        {
            WL_CHECK(start <= end);
        }

        constexpr Iterator begin() const noexcept
        {
            return Iterator(m_start);
        }

        constexpr Iterator end() const noexcept
        {
            return Iterator(m_end);
        }

        constexpr const Iterator cbegin() const noexcept
        {
            return begin();
        }

        constexpr const Iterator cend() const noexcept
        {
            return end();
        }

        constexpr IndexType Size() const noexcept
        {
            return m_end - m_start;
        }

    private:
        IndexType m_start;
        IndexType m_end;
    };

}// namespace Wl