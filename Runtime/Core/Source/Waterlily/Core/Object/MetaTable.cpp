#include "MetaTable.hpp"

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

        MemberInfo info = {};
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

    const MemberInfo& MetaTable::GetMemberInfo(Type type, const StringID& name)
    {
        MetaTable& table = MetaTable::Get();

        OrderedSet<MemberInfo>& infos = table.memberInfoMap[type];
        HashMap<StringID, uint32_t>& map = table.nameToOffsetMap[type];
        uint32_t offset = map[name];

        MemberInfo info;
        info.offset = offset;

        return *infos.find(info);
    }

}// namespace Wl