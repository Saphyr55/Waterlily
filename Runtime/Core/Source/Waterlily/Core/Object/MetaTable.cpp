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

    Member MetaTable::RegisterMember(Type type, const TypeDescriptor& variable, const StringID& name, uint32_t offset, uint32_t size, uint32_t align)
    {
        MetaTable& table = MetaTable::Get();

        Member member(type, offset);

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

        return member;
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


}// namespace Wl