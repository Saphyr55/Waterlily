#pragma once

#include "Waterlily/Core/Containers/HashMap.hpp"
#include "Waterlily/Core/CoreExports.hpp"
#include "Waterlily/Core/Object/TypeInfo.hpp"
#include "Waterlily/Core/String/StringID.hpp"
#include <cstdint>

namespace Wl
{

    class WL_CORE_API Type
    {
    public:
        using IdentifierType = uint64_t;

    public:
        template<typename T, typename InheritType = void>
        inline static Type Of()
        {
            if (Type::Contains<T>())
            {
                return Type(TypeID<T>());
            }
            return Type::Register<T, InheritType>();
        }

        template<typename T, typename InheritType = void>
        inline static Type Register() noexcept
        {
            TypeInfo info = TypeInfo::Of<T, InheritType>();
            GetRegistry().infos.Emplace(info.name.GetHash(), info);
            return Type(TypeID<T>());
        }

        template<typename T>
        inline static bool Contains()
        {
            return Contains(TypeID<T>());
        }

        inline static bool Contains(const StringID& name)
        {
            return Contains(name.GetHash());
        }

        inline static bool Contains(IdentifierType id)
        {
            return GetRegistry().infos.Contains(id);
        }

        template<typename T>
        inline static const TypeInfo& GetTypeInfo()
        {
            return GetTypeInfo(TypeID<T>());
        }

        inline static const TypeInfo& GetTypeInfo(const StringID& name)
        {
            return GetTypeInfo(name.GetHash());
        }

        inline static const TypeInfo& GetTypeInfo(IdentifierType id)
        {
            return GetRegistry().infos.Get(id);
        }

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

        inline const TypeInfo& GetTypeInfo() const
        {
            return GetRegistry().infos.Get(m_id);
        }

        inline bool InheritFrom(Type parentType) const
        {
            const StringID& target = parentType.GetName();
            const StringID& rootName = Type::Of<void>().GetName();

            StringID current = GetName();
            while (current != rootName)
            {
                if (current == target)
                    return true;

                const TypeInfo& info = Type::GetTypeInfo(current);
                current = info.inherit;
            }
            return false;
        }

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
        struct Registry
        {
            HashMap<IdentifierType, TypeInfo> infos;
        };

        static Registry& GetRegistry();

        IdentifierType m_id;
    };

}// namespace Wl

WL_HASH_DEFINE(Wl::Type, type, {
    return type.GetID();
})