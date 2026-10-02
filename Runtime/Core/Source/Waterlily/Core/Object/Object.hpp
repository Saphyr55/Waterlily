#pragma once

#include "Waterlily/Core/CoreExports.hpp"
#include "Waterlily/Core/Object/MetaTable.hpp"
#include <cstdio>

#define WL_DETAIL_OBJECT_AUTO_REGISTRANT(TypeName, InheritTypeName) \
    AutoRegistrantObjectType<TypeName, InheritTypeName> _AutoRegistrant##TypeName

#define WL_DETAIL_OBJECT(TypeName, InheritTypeName)                                  \
private:                                                                             \
    inline static const WL_DETAIL_OBJECT_AUTO_REGISTRANT(TypeName, InheritTypeName); \
                                                                                     \
                                                                                     \
public:                                                                              \
    inline static ::Wl::Type StaticType()                                            \
    {                                                                                \
        return ::Wl::MetaTable::TypeOf<TypeName, InheritTypeName>();                 \
    }                                                                                \
                                                                                     \
    virtual ::Wl::Type GetObjectType() const                                         \
    {                                                                                \
        return StaticType();                                                         \
    }                                                                                \
                                                                                     \
private:                                                                             \
    static void _RegisterBindings();                                                 \
    friend class ::Wl::MetaTable;


#define WL_OBJECT(TypeName, InheritTypeName) \
    using Self = TypeName;                   \
    using Super = InheritTypeName;           \
    WL_DETAIL_OBJECT(TypeName, InheritTypeName)

#define WL_OBJECT_ROOT(TypeName) \
    using Self = TypeName;       \
    WL_DETAIL_OBJECT(TypeName, void)

namespace Wl
{

    template<typename ObjectType, typename InheritType>
    class AutoRegistrantObjectType
    {
    public:
        AutoRegistrantObjectType()
        {
            Type type = MetaTable::TypeOf<ObjectType, InheritType>();
        }
    };

    class WL_CORE_API Object
    {
        WL_OBJECT_ROOT(Object);
    };

}// namespace Wl
