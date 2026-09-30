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

    void MetaTable::RegisterMemberImpl(
            Type type,
            const TypeDescriptor& variable,
            const StringID& name,
            uint32_t offset,
            uint32_t size,
            uint32_t align)
    {
        MetaTable& table = MetaTable::Get();

        MemberInfo info;
        info.owner = type;
        info.name = name;
        info.variable = variable;
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
        MetaTable& table = MetaTable::Get();
        TypeTable& typeTable = table.typeTableMap.Get(type);
        return Member(type, typeTable.nameToOffsetMap.Get(name));
    }

    const MemberInfo& MetaTable::GetMemberInfo(Type type, const StringID& name)
    {
        MetaTable& table = MetaTable::Get();
        TypeTable& typeTable = table.typeTableMap.Get(type);

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
        MetaTable& table = Get();
        TypeTable& typeTable = table.typeTableMap.Get(type);
        return typeTable.methodMap.Get(name);
    }

    void MetaTable::RegisterProperty(
            Type type,
            const StringID& name,
            const StringID& getterName,
            const StringID& setterName)
    {
        MetaTable& table = MetaTable::Get();
        WL_CHECK(table.typeTableMap.Contains(type));

        TypeTable& typeTable = table.typeTableMap.Get(type);

        WL_CHECK_MSG(
                typeTable.nameToOffsetMap.Contains(name),
                "'%s' member has been not registered.",
                name.GetText().GetData());

        PropertyInfo info;
        info.name = name;
        info.owner = type;
        info.getterMethodName = getterName;
        info.setterMethodName = setterName;

        typeTable.propertyMap.Put(name, info);
    }

    bool MetaTable::ConstainsProperty(Type type, const StringID& name)
    {
        const TypeTable& typeTable = Get().typeTableMap.Get(type);
        return typeTable.propertyMap.Contains(name);
    }

    const PropertyInfo& MetaTable::GetPropertyInfo(Type type, const StringID& name)
    {
        const TypeTable& typeTable = Get().typeTableMap.Get(type);
        return typeTable.propertyMap.Get(name);
    }

}// namespace Wl
