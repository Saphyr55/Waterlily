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

        inline size_t GetSize() const
        {
            return GetTypeInfo().size;
        }

        inline size_t GetAlign() const
        {
            return GetTypeInfo().align;
        }

        constexpr inline IdentifierType GetID() const
        {
            return m_id;
        }

        const TypeInfo& GetTypeInfo() const;
        
        bool InheritFrom(Type parentType) const;

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
        constexpr explicit Type(IdentifierType id = TypeID<void>()) noexcept;
        constexpr Type(const Type&) noexcept = default;
        Type(Type&&) noexcept = default;
        constexpr ~Type() noexcept = default;

        constexpr Type& operator=(const Type& other) noexcept = default;
        Type& operator=(Type&& other) noexcept = default;

    private:
        IdentifierType m_id;
    };

}// namespace Wl

WL_HASH_DEFINE(Wl::Type, type, {
    return type.GetID();
})