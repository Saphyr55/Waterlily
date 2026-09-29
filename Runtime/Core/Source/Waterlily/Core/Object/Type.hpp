#pragma once

#include "Waterlily/Core/Containers/HashMap.hpp"
#include "Waterlily/Core/CoreExports.hpp"
#include "Waterlily/Core/Object/TypeInfo.hpp"

namespace Wl
{

    class WL_CORE_API Type
    {
    public:
        template<typename T>
        inline static Type Register()
        {
            TypeInfo info = TypeInfo::Create<T>();
            GetRegistry().infos.Emplace(info.Name.GetHash(), info);
            return Type(info.Name.GetHash());
        }

        template<typename T>
        inline static bool Contains()
        {
            return GetRegistry().infos.Contains(TypeID<T>());
        }

        template<typename T>
        inline static const TypeInfo& GetTypeInfo()
        {
            return GetRegistry().infos.Get(TypeID<T>());
        }

    public:
        constexpr inline uint64_t GetID() const
        {
            return m_id;
        }

        inline const TypeInfo& GetTypeInfo() const
        {
            return GetRegistry().infos.Get(m_id);
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
        constexpr explicit Type(uint64_t id);
        constexpr Type(const Type&) = default;
        constexpr Type(Type&&) = default;
        constexpr ~Type() = default;

        constexpr Type& operator=(const Type& other) = default;
        constexpr Type& operator=(Type&& other) = default;

    private:
        struct Registry
        {
            HashMap<uint64_t, TypeInfo> infos;
        };

        static Registry& GetRegistry();

        uint64_t m_id;
    };

    template<typename T>
    inline Type TypeOf()
    {
        if (Type::Contains<T>())
        {
            return Type(TypeID<T>());
        }
        return Type::Register<T>();
    }

    class TypeBuilder
    {
    };

}// namespace Wl

WL_HASH_DEFINE(Wl::Type, type, {
    return type.GetID();
})