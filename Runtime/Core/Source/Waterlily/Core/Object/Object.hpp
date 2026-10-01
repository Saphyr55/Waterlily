#pragma once

#include "Waterlily/Core/CoreExports.hpp"
#include "Waterlily/Core/Object/MetaTable.hpp"

#define _WL_OBJECT_NAME_PREFIX(Suffix) _Object##Suffix
#define _WL_OBJECT_NAME_TYPE(Name) _WL_OBJECT_NAME_PREFIX(Name##Type)

#define _WL_OBJECT_AUTO_REGISTRANT(TypeName, InheritTypeName) \
    AutoRegistrantObjectType<TypeName, InheritTypeName>       \
    _WL_OBJECT_NAME_TYPE(_AutoRegistrant##TypeName)

#define _WL_OBJECT(TypeName, InheritTypeName)                                  \
private:                                                                       \
    inline static const _WL_OBJECT_AUTO_REGISTRANT(TypeName, InheritTypeName); \
                                                                               \
                                                                               \
public:                                                                        \
    inline static ::Wl::Type StaticType()                                      \
    {                                                                          \
        return ::Wl::MetaTable::TypeOf<TypeName, InheritTypeName>();           \
    }                                                                          \
                                                                               \
    virtual ::Wl::Type GetObjectType() const                                   \
    {                                                                          \
        return StaticType();                                                   \
    }                                                                          \
                                                                               \
private:                                                                       \
    static void _RegisterBindings();                                           \
    friend class ::Wl::MetaTable;


#define WL_OBJECT(TypeName, InheritTypeName) \
    using Self = TypeName;                   \
    using Super = InheritTypeName;           \
    _WL_OBJECT(TypeName, InheritTypeName)

#define WL_TYPE(TypeName)  \
    using Self = TypeName; \
    _WL_OBJECT(TypeName, void)

namespace Wl
{

    template<typename ObjectType, typename InheritType>
    class AutoRegistrantObjectType
    {
    public:
        AutoRegistrantObjectType()
        {
            ::Wl::MetaTable::TypeOf<ObjectType, InheritType>();
        }
    };

    class WL_CORE_API Object
    {
        WL_TYPE(Object);
    };

}// namespace Wl
