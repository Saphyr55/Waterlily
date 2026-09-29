#include "Property.hpp"
#include "Waterlily/Core/Containers/HashMap.hpp"
#include "Waterlily/Core/Containers/Set.hpp"
#include "Waterlily/Core/String/StringID.hpp"
#include <cstdint>

namespace Wl
{

    Property Property::Register(Type type, const Variable& variable, const StringID& name, uint32_t offset, uint32_t size, uint32_t align)
    {
        Property property(type, offset);

        PropertyInfo info = {};
        info.name = name;
        info.variable = variable;
        info.align = align;
        info.size = size;
        info.offset = offset;

        if (!GetRegistry().nameToOffset.Contains(type))
        {
            GetRegistry().nameToOffset.Put(type, HashMap<StringID, uint32_t>());
            GetRegistry().infos.Put(type, OrderedSet<PropertyInfo>());
        }

        HashMap<StringID, uint32_t>& map = GetRegistry().nameToOffset.Get(type);
        OrderedSet<PropertyInfo>& infos = GetRegistry().infos.Get(type);

        if (!map.Contains(name))
        {
            infos.insert(info);
            map.Put(name, offset);
        }

        return property;
    }

    const PropertyInfo& Property::GetPropertyInfo(Type type, const StringID& name)
    {
        OrderedSet<PropertyInfo>& infos = GetRegistry().infos[type];
        HashMap<StringID, uint32_t>& map = GetRegistry().nameToOffset[type];
        uint32_t offset = map[name];

        PropertyInfo info;
        info.offset = offset;

        return *infos.find(info);
    }

    Property::Registry& Property::GetRegistry()
    {
        static Property::Registry registry;
        return registry;
    }

}// namespace Wl