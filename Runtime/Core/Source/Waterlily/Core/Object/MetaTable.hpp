#pragma once

#include "Waterlily/Core/Containers/HashMap.hpp"
#include "Waterlily/Core/Containers/Set.hpp"
#include "Waterlily/Core/CoreExports.hpp"
#include "Waterlily/Core/Object/Member.hpp"
#include "Waterlily/Core/Object/MemberInfo.hpp"

namespace Wl
{

    class WL_CORE_API MetaTable
    {
    public:
        template<typename T, typename InheritType = void>
        inline static Type TypeOf()
        {
            if (ContainsType<T>())
            {
                return Type(TypeID<T>());
            }
            
            MetaTable& table = Get();
            TypeInfo info = TypeInfo::Of<T, InheritType>();
            table.infos.Put(info.name.GetHash(), info);
            
            return Type(TypeID<T>());
        }

        template<typename T>
        constexpr inline static Variable VariableOf()
        {
            using DetailsType = VariableDetails<T>;

            Variable var(TypeOf<typename DetailsType::PlainType>());

            if constexpr (std::is_reference_v<T>)
            {
                var.SetReference();
            }

            if constexpr (std::is_rvalue_reference_v<T>)
            {
                var.SetRValueReference();
            }

            if constexpr (std::is_const_v<typename DetailsType::RemovedPointersType>)
            {
                var.SetConst();
            }

            if constexpr (std::is_volatile_v<typename DetailsType::RemovedPointersType>)
            {
                var.SetVolatile();
            }

            var.SetArraySize(DetailsType::GetArraySize());
            var.SetPointerCount(DetailsType::GetPointerCount());

            return var;
        }

        template<>
        constexpr inline static Variable VariableOf<void>()
        {
            return Variable();
        }

        template<typename T>
        inline static bool ContainsType()
        {
            return ContainsType(TypeID<T>());
        }

        inline static bool ContainsType(const StringID& name)
        {
            return ContainsType(name.GetHash());
        }

        inline static bool ContainsType(IdentifierType id)
        {
            return Get().infos.Contains(id);
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
            return Get().infos.Get(id);
        }

        template<typename MemberType, typename ObjectType>
        static Member RegisterMember(const StringID& name, MemberType ObjectType::* member);

        static Member RegisterMember(Type objectType, const Variable& variable, const StringID& name, uint32_t offset, uint32_t size, uint32_t align);

        static const MemberInfo& GetMemberInfo(Type type, const StringID& name);

    public:
        HashMap<IdentifierType, TypeInfo> infos;
        HashMap<Type, OrderedSet<MemberInfo>> memberInfoMap;
        HashMap<Type, HashMap<StringID, uint32_t>> nameToOffsetMap;

    public:
        static MetaTable& Get();
    };

    template<typename PropertyType, typename ObjectType>
    Member MetaTable::RegisterMember(const StringID& name, PropertyType ObjectType::* property)
    {
        Type objectType = MetaTable::TypeOf<ObjectType>();
        size_t offset = MemberOffset(property);
        Variable variable = MetaTable::VariableOf<PropertyType>();
        size_t size = sizeof(PropertyType);
        size_t align = alignof(PropertyType);
        return RegisterMember(objectType, variable, name, offset, size, align);
    }

}// namespace Wl