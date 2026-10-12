#include "MetaTable.hpp"
#include "Member.hpp"
#include "MemberInfo.hpp"
#include "Waterlily/Core/Containers/HashMap.hpp"

namespace Wl
{

    MetaTable& MetaTable::Get()
    {
        static MetaTable table;
        return table;
    }

    void MetaTable::Init()
    {
        MetaTable& table = Get();
        for (const auto& [type, typeTable]: table.typeTableMap)
        {
            if (typeTable.registerBindings)
            {
                typeTable.registerBindings();
            }
        }
    }

    void MetaTable::Shutdown()
    {
    }

    void MetaTable::RegisterMemberImpl(
            Type type,
            const TypeDescriptor& typeDesc,
            const StringID& name,
            uint32 offset,
            uint32 size,
            uint32 align)
    {
        MetaTable& table = MetaTable::Get();

        MemberInfo info;
        info.owner = type;
        info.name = name;
        info.typeDesc = typeDesc;
        info.align = align;
        info.size = size;
        info.offset = offset;

        TypeTable& typeTable = table.typeTableMap.Get(type);
        if (!typeTable.nameToOffsetMap.Contains(name))
        {
            typeTable.memberInfoMap.insert(info);
            typeTable.nameToOffsetMap.Put(name, offset);
        }
    }

    Member MetaTable::GetMember(Type type, const StringID& name)
    {
        TypeTable& typeTable = GetTypeTable(type);
        return Member(type, typeTable.nameToOffsetMap.Get(name));
    }

    const MemberInfo& MetaTable::GetMemberInfo(Type type, const StringID& name)
    {
        TypeTable& typeTable = GetTypeTable(type);

        Member member = GetMember(type, name);

        MemberInfo info;
        info.name = name;
        info.owner = type;
        info.offset = member.GetOffset();

        return *typeTable.memberInfoMap.find(info);
    }

    bool MetaTable::ContainsMember(Type type, const StringID& name)
    {
        if (auto* ptr = MetaTable::Get().typeTableMap.GetPtr(type))
        {
            return ptr->nameToOffsetMap.Contains(name);
        }
        return false;
    }

    bool MetaTable::ConstainsMethod(Type type, const StringID& name)
    {
        if (auto* ptr = MetaTable::Get().typeTableMap.GetPtr(type))
        {
            return ptr->methodMap.Contains(name);
        }
        return false;
    }

    const MethodInfo& MetaTable::GetMethodInfo(Type type, const StringID& name)
    {
        return GetTypeTable(type).methodMap.Get(name);
    }

    void MetaTable::RegisterProperty(
            Type type,
            const StringID& name,
            const StringID& getterName,
            const StringID& setterName)
    {
        MetaTable& table = MetaTable::Get();
        TypeTable& typeTable = GetTypeTable(type);

        WL_CHECK(typeTable.nameToOffsetMap.Contains(name));

        PropertyInfo info;
        info.name = name;
        info.owner = type;
        info.getterMethodName = getterName;
        info.setterMethodName = setterName;

        typeTable.propertyMap.Put(name, info);
    }

    bool MetaTable::ConstainsProperty(Type type, const StringID& name)
    {
        return GetProperties(type).Contains(name);
    }

    const PropertyInfo& MetaTable::GetPropertyInfo(Type type, const StringID& name)
    {
        return GetProperties(type).Get(name);
    }

    TypeTable& MetaTable::GetTypeTable(Type type)
    {
        MetaTable& meta = Get();
        WL_CHECK(meta.typeTableMap.Contains(type));
        return meta.typeTableMap.Get(type);
    }

    HashMap<IdentifierType, TypeInfo>& MetaTable::GetTypes()
    {
        return Get().types;
    }

    const OrderedSet<MemberInfo>& MetaTable::GetMembers(Type type)
    {
        return GetTypeTable(type).memberInfoMap;
    }

    const HashMap<StringID, PropertyInfo>& MetaTable::GetProperties(Type type)
    {
        return GetTypeTable(type).propertyMap;
    }

    const HashMap<StringID, MethodInfo>& MetaTable::GetMethods(Type type)
    {
        return GetTypeTable(type).methodMap;
    }

}// namespace Wl
