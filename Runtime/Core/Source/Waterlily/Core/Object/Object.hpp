#pragma once

#include "Waterlily/Core/Object/Type.hpp"

#define _WL_OBJECT_NAME_PREFIX(Suffix) _Object##Suffix
#define _WL_OBJECT_NAME_TYPE(Name) _WL_OBJECT_NAME_PREFIX(Name##Type)

#define _WL_OBJECT_BINDING_METHOD_NAME() _WL_OBJECT_NAME_PREFIX(RegisterBindings())

#define _WL_OBJECT(TypeName, InheritTypeName)                       \
private:                                                            \
    const inline static ::Wl::Type _WL_OBJECT_NAME_TYPE(TypeName) = \
            ::Wl::Type::Register<TypeName, InheritTypeName>();      \
                                                                    \
public:                                                             \
    inline static ::Wl::Type StaticType()                           \
    {                                                               \
        return _WL_OBJECT_NAME_TYPE(TypeName);                      \
    }                                                               \
                                                                    \
    virtual ::Wl::Type GetObjectType() const                        \
    {                                                               \
        return StaticType();                                        \
    }                                                               \
                                                                    \
private:                                                            \
    static void _WL_OBJECT_BINDING_METHOD_NAME();


#define WL_OBJECT(TypeName, InheritTypeName) \
    using Self = TypeName;                   \
    using Super = InheritTypeName;           \
    _WL_OBJECT(TypeName, InheritTypeName)

#define WL_OBJECT_ROOT(TypeName) _WL_OBJECT(TypeName, void)

namespace Wl
{

    class Object
    {
        WL_OBJECT_ROOT(Object);
    };

}// namespace Wl
