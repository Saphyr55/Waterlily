#pragma once

#include "Waterlily/Core/Memory/SharedPtr.hpp"

#include <concepts>

namespace Wl
{

    template<typename To, typename From>
        requires(std::derived_from<To, From>)
    inline SharedPtr<To> StaticPtrCast(const SharedPtr<From>& other) noexcept
    {
        To* to = static_cast<To*>(other.GetResource());
        return SharedPtr<To>(to, other.GetReferenceCounter());
    }

    template<typename To, typename From>
        requires(std::derived_from<To, From>)
    inline SharedPtr<To> ReinterpretPtrCast(const SharedPtr<From>& other) noexcept
    {
        To* to = reinterpret_cast<To*>(other.GetResource());
        return SharedPtr<To>(to, other.GetReferenceCounter());
    }

    template<typename To, typename From>
        requires(std::derived_from<To, From>)
    inline SharedPtr<To> DynamicPtrCast(const SharedPtr<From>& other) noexcept
    {
        To* to = dynamic_cast<To*>(other.GetResource());
        return SharedPtr<To>(to, other.GetReferenceCounter());
    }

    template<typename To, typename From>
        requires(std::derived_from<To, From>)
    inline SharedPtr<To> ConstPtrCast(const SharedPtr<From>& other) noexcept
    {
        To* to = const_cast<To*>(other.GetResource());
        return SharedPtr<To>(to, other.GetReferenceCounter());
    }

}// namespace Wl