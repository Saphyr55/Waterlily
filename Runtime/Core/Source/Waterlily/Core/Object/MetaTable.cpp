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

    Member MetaTable::RegisterMember(Type type, const Variable& variable, const StringID& name, uint32_t offset, uint32_t size, uint32_t align)
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

        if (!table.nameToOffsetMap.Contains(type))
        {
            table.nameToOffsetMap.Put(type, HashMap<StringID, uint32_t>());
            table.memberInfoMap.Put(type, OrderedSet<MemberInfo>());
        }

        HashMap<StringID, uint32_t>& map = table.nameToOffsetMap.Get(type);
        OrderedSet<MemberInfo>& infos = table.memberInfoMap.Get(type);

        if (!map.Contains(name))
        {
            infos.insert(info);
            map.Put(name, offset);
        }

        return member;
    }

    Member MetaTable::GetMember(Type type, const StringID& name)
    {
        MetaTable& table = MetaTable::Get();
        HashMap<StringID, uint32_t>& map = table.nameToOffsetMap.Get(type);
        return Member(type, map.Get(name));
    }

    const MemberInfo& MetaTable::GetMemberInfo(Type type, const StringID& name)
    {
        MetaTable& table = MetaTable::Get();

        OrderedSet<MemberInfo>& infos = table.memberInfoMap.Get(type);
        Member member = GetMember(type, name);

        MemberInfo info;
        info.name = name;
        info.owner = type;
        info.offset = member.GetOffset();

        return *infos.find(info);
    }

    bool MetaTable::ContainsMember(Type type, const StringID& name)
    {
        if (auto* ptr = MetaTable::Get().nameToOffsetMap.GetPtr(type))
        {
            return ptr->Contains(name);
        }
        return false;
    }
    
    bool MetaTable::ConstainsMethod(Type type, const StringID& name)
    {
        if (auto* ptr = MetaTable::Get().methodMap.GetPtr(type))
        {
            return ptr->Contains(name);
        }
        return false; 
    }

    const MethodInfo& MetaTable::GetMethodInfo(Type type, const StringID& name)
    {
        MetaTable& table = Get();

        if (!table.methodMap.Contains(type))
        {
            table.methodMap.Put(type, HashMap<StringID, MethodInfo>());
        }

        HashMap<StringID, MethodInfo>& methods = table.methodMap.Get(type);

        return methods.Get(name);
    }


}// namespace Wl