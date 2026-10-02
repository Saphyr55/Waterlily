#include "Waterlily/Core/Object/Type.hpp"
#include "MetaTable.hpp"

namespace Wl
{

    const TypeInfo& Type::GetTypeInfo() const
    {
        return MetaTable::Get().GetTypeInfo(m_id);
    }

    bool Type::InheritFrom(Type parentType) const
    {
        const StringID& target = parentType.GetName();
        const StringID& rootName = MetaTable::TypeOf<void>().GetName();

        StringID current = GetName();
        while (current != rootName)
        {
            if (current == target)
                return true;

            const TypeInfo& info = MetaTable::GetTypeInfo(current);
            current = info.inherit;
        }
        return false;
    }
    
    Type Type::GetParent() const
    {
        return MetaTable::GetType(GetTypeInfo().inherit);
    }

    constexpr Type::Type(IdentifierType id) noexcept
        : m_id(id)
    {
    }

}// namespace Wl