#pragma once

#include "Waterlily/Core/CoreExports.hpp"
#include "Waterlily/Core/Object/TypeInfo.hpp"
#include "Waterlily/Core/String/StringID.hpp"

namespace Wl
{

    class WL_CORE_API Type
    {
    public:
        inline const StringID& GetName() const
        {
            return GetTypeInfo().name;
        }

        inline usize GetSize() const
        {
            return GetTypeInfo().size;
        }

        inline usize GetAlign() const
        {
            return GetTypeInfo().align;
        }

        constexpr inline IdentifierType GetID() const
        {
            return m_id;
        }

        const TypeInfo& GetTypeInfo() const;

        bool InheritFrom(Type parentType) const;

        Type GetParent() const;

    public:
        constexpr bool operator==(const Type& other) const
        {
            return m_id == other.m_id;
        }

        constexpr bool operator!=(const Type& other) const
        {
            return !(*this == other);
        }

    public:
        constexpr Type() noexcept;
        constexpr explicit Type(IdentifierType id) noexcept;
        constexpr Type(const Type&) noexcept = default;
        constexpr Type(Type&&) noexcept = default;
        constexpr ~Type() noexcept = default;

        constexpr Type& operator=(const Type& other) noexcept = default;
        Type& operator=(Type&& other) noexcept = default;

    private:
        IdentifierType m_id;
    };

    template<typename T>
    class TypedType : public Type
    {
    public:
        constexpr explicit TypedType(IdentifierType id) noexcept
            : Type(id)
        {
        }
        constexpr TypedType(Type type) noexcept
            : TypedType(type.GetID())
        {
        }
        constexpr TypedType(const TypedType&) noexcept = default;
        constexpr TypedType(TypedType&&) noexcept = default;
        constexpr ~TypedType() noexcept = default;

        constexpr TypedType& operator=(const TypedType& other) noexcept = default;
        constexpr TypedType& operator=(TypedType&& other) noexcept = default;
    };

}// namespace Wl

WL_HASH_DEFINE(Wl::Type, type, {
    return type.GetID();
})