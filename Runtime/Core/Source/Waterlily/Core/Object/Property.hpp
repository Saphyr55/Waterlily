#pragma once

#include "Variable.hpp"
#include "Waterlily/Core/Containers/HashMap.hpp"
#include "Waterlily/Core/Containers/Set.hpp"
#include "Waterlily/Core/Memory/Memory.hpp"
#include "Waterlily/Core/String/StringID.hpp"

#include <cstdint>

namespace Wl
{

    struct PropertyInfo
    {
        StringID name;
        Variable variable;
        uint32_t offset;
        uint32_t size;
        uint32_t align;

        // For the ordered set.
        constexpr bool operator<(const PropertyInfo& rhs) const
        {
            return offset < rhs.offset;
        }
    };

    class WL_CORE_API Property
    {
    public:
        template<typename PropertyType, typename ObjectType>
        static Property Register(const StringID& name, PropertyType ObjectType::* property);

        static Property Register(Type objectType, const Variable& variable, const StringID& name, uint32_t offset, uint32_t size, uint32_t align);

        static const PropertyInfo& GetPropertyInfo(Type type, const StringID& name);

    public:
        constexpr Property() = default;
        constexpr explicit Property(Type type, uint32_t offset)
            : m_type(type)
            , m_offset(offset)
        {
        }
        constexpr Property(const Property&) = default;
        Property(Property&&) noexcept = default;
        constexpr ~Property() = default;

        constexpr Property& operator=(const Property&) = default;
        Property& operator=(Property&&) noexcept = default;

    private:
        struct Registry
        {
            HashMap<Type, OrderedSet<PropertyInfo>> infos;
            HashMap<Type, HashMap<StringID, uint32_t>> nameToOffset;
        };

        static Registry& GetRegistry();

        Type m_type;
        uint32_t m_offset = 0;
    };
    
    template<class ObjectType, class MemberType>
    inline size_t MemberOffset(MemberType ObjectType::* member)
    {
        AlignedStorageType<ObjectType> buffer;
        const uint8_t* base = reinterpret_cast<const uint8_t*>(&(buffer.GetPtr()->*member));
        return base - buffer.Storage;
    }

    template<typename T>
    inline size_t MemberOffsetAlignUp(size_t offset, size_t alignment, size_t& outMemberOffset)
    {
        offset = Memory::AlignUp(offset, alignment);
        outMemberOffset = offset;
        offset += sizeof(T);
        return offset;
    }

    template<typename PropertyType, typename ObjectType>
    Property Property::Register(const StringID& name, PropertyType ObjectType::* property)
    {
        Type objectType = Type::Of<ObjectType>();
        size_t offset = MemberOffset(property);
        Variable variable = Variable::Of<PropertyType>();
        size_t size = sizeof(PropertyType);
        size_t align = alignof(PropertyType);
        return Register(objectType, variable, name, offset, size, align);
    }

}// namespace Wl
