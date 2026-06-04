#pragma once

#include "Waterlily/Core/Defines.hpp"
#include "Waterlily/Core/Memory/Memory.hpp"
#include "Waterlily/Core/Traits/AlignedStorage.hpp"

#include <type_traits>

namespace Wl
{

    template<typename T>
    class Option
    {
    public:
        constexpr static Option<T> Some(const T& value)
        {
            return Option<T>(value);
        }

        constexpr static Option<T> None()
        {
            return Option<T>();
        }

        constexpr Option(const T& value)
            : m_isSome(true)
        {
            WL_PLACEMENT_NEW(&m_storage)
            T(value);
        }

        constexpr Option()
            : m_isSome(false)
        {
        }

        constexpr Option(const Option& other)
            : m_isSome(other.m_isSome)
        {
            if (m_isSome)
            {
                WL_PLACEMENT_NEW(&m_storage)
                T(*other.GetData());
            }
        }

        constexpr Option& operator=(const Option& other)
        {
            if (this != &other)
            {
                if (m_isSome)
                {
                    GetData()->~T();
                }
                m_isSome = other.m_isSome;
                if (m_isSome)
                {
                    WL_PLACEMENT_NEW(&m_storage)
                    T(*other.GetData());
                }
            }
            return *this;
        }

        constexpr ~Option()
        {
            if (m_isSome)
            {
                GetData()->~T();
            }
        }

        constexpr bool HasValue() const
        {
            return m_isSome;
        }

        constexpr T Unwrap() const
        {
            WL_CHECK(HasValue());
            return *GetData();
        }

        constexpr T UnwrapOr(const T& default_value) const
        {
            return HasValue() ? *GetData() : default_value;
        }

        const T& operator*() const
        {
            WL_CHECK(HasValue());
            return *GetData();
        }

        T& operator*()
        {
            WL_CHECK(HasValue());
            return *GetData();
        }

        const T& operator->() const
        {
            WL_CHECK(HasValue());
            return *GetData();
        }

        T* operator->()
        {
            WL_CHECK(HasValue());
            return GetData();
        }

        explicit operator bool() const
        {
            return HasValue();
        }

    private:
        const T* GetData() const
        {
            return reinterpret_cast<const T*>(&m_storage);
        }

        T* GetData()
        {
            return reinterpret_cast<T*>(&m_storage);
        }

    private:
        AlignedStorageType<T> m_storage;
        bool m_isSome;
    };

    auto OptionSome(const auto& value) -> auto
    {
        using const_value_type = std::remove_reference_t<decltype(value)>;
        using value_type = std::remove_const_t<const_value_type>;

        return Option<value_type>::Some(value);
    }

    template<typename T>
    auto OptionNone() -> auto
    {
        return Option<T>::None();
    }

}// namespace Wl