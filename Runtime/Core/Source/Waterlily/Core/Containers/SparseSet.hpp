#pragma once

#include "Waterlily/Core/Containers/Array.hpp"

#include <limits>

namespace Wl
{

    template<typename ElementType, typename KeyIndexType = usize>
    class SparseSet
    {
    public:
        using size_type = usize;
        using value_type = ElementType;
        using reference = ElementType&;
        using const_reference = const ElementType&;
        using pointer = ElementType*;
        using const_pointer = const ElementType*;

        using SparseType = Array<KeyIndexType>;
        using DenseType = Array<value_type>;

        static constexpr const KeyIndexType s_InvalidIndex = std::numeric_limits<KeyIndexType>::max();

    public:
        pointer Put(size_type i, const_reference element)
        {
            KeyIndexType index = GetDenseIndex(i);
            if (index != s_InvalidIndex)
            {
                m_denseValue[index] = element;
                m_denseIdentification[index] = i;

                return &m_denseValue[index];
            }

            KeyIndexType new_index = m_denseValue.GetSize();
            SetDenseIndex(i, new_index);

            m_denseValue.Append(element);
            m_denseIdentification.Append(i);

            return &m_denseValue.Back();
        }

        reference operator[](size_type i)
        {
            KeyIndexType index = GetDenseIndex(i);
            return m_denseValue[index];
        }

        const_reference operator[](size_type i) const
        {
            KeyIndexType index = GetDenseIndex(i);
            return m_denseValue[index];
        }

        pointer Get(size_type i)
        {
            KeyIndexType index = GetDenseIndex(i);
            return (index != s_InvalidIndex) ? &m_denseValue[index] : nullptr;
        }

        const_pointer Get(size_type i) const
        {
            KeyIndexType index = GetDenseIndex(i);
            return (index != s_InvalidIndex) ? &m_denseValue[index] : nullptr;
        }

        void Remove(size_type i)
        {
            KeyIndexType deletedIndex = GetDenseIndex(i);
            WL_CHECK(deletedIndex != s_InvalidIndex && !m_denseValue.IsEmpty());

            KeyIndexType lastIndex = m_denseValue.GetSize() - 1;
            KeyIndexType lastId = m_denseIdentification.Back();

            if (deletedIndex != lastIndex)
            {
                std::swap(m_denseValue[deletedIndex], m_denseValue.Back());
                std::swap(m_denseIdentification[deletedIndex], m_denseIdentification.Back());
                SetDenseIndex(lastId, deletedIndex);
            }

            SetDenseIndex(i, s_InvalidIndex);

            m_denseValue.Pop();
            m_denseIdentification.Pop();
        }

        SparseType& GetDense()
        {
            return m_denseIdentification;
        }

        KeyIndexType GetDenseAt(size_type denseIndex) const
        {
            return m_denseIdentification[denseIndex];
        }

        inline const DenseType& GetElements() const
        {
            return m_denseValue;
        }

        inline bool Contains(size_type i)
        {
            return GetDenseIndex(i) != s_InvalidIndex;
        }

        inline bool IsEmpty() const
        {
            return m_denseValue.IsEmpty();
        }

        inline size_type GetSize() const
        {
            return m_denseValue.GetSize();
        }

        void Reserve(size_type denseCapacity)
        {
            if (m_capacityPage < denseCapacity)
            {
                m_capacityPage = denseCapacity;
            }
            m_denseValue.Reserve(denseCapacity);
            m_denseIdentification.Reserve(denseCapacity);
        }

        void Resize(size_type newSize, const_reference defaultValue = ElementType{})
        {
            if (m_capacityPage < newSize)
            {
                m_capacityPage = newSize;
            }
            m_denseValue.Resize(newSize, defaultValue);
            m_denseIdentification.Resize(newSize, s_InvalidIndex);
        }

        void Clear()
        {
            m_denseValue.Clear();
            m_denseIdentification.Clear();
            m_sparses.Clear();
        }

    public:
        SparseSet(usize capacityPage = 100)
            : m_capacityPage(capacityPage)
            , m_denseValue(capacityPage)
            , m_sparses(capacityPage)
        {
        }

        ~SparseSet() = default;

    private:
        void SetDenseIndex(size_type i, KeyIndexType index)
        {
            size_type page = i / m_capacityPage;
            size_type sparseIndex = i % m_capacityPage;

            if (page >= m_sparses.GetSize())
            {
                m_sparses.Resize(page + 1);
            }

            SparseType& sparse = m_sparses[page];
            if (sparseIndex >= sparse.GetSize())
            {
                sparse.Resize(sparseIndex + 1, s_InvalidIndex);
            }

            sparse[sparseIndex] = index;
        }

        KeyIndexType GetDenseIndex(size_type i) const
        {
            size_type page = i / m_capacityPage;
            size_type sparseIndex = i % m_capacityPage;

            if (page < m_sparses.GetSize())
            {
                const SparseType& sparse = m_sparses[page];
                if (sparseIndex < sparse.GetSize())
                {
                    return sparse[sparseIndex];
                }
            }

            return s_InvalidIndex;
        }

    private:
        Array<SparseType> m_sparses;
        DenseType m_denseValue;
        SparseType m_denseIdentification;
        size_type m_capacityPage;
    };

}// namespace Wl