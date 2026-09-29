#pragma once

#include "Waterlily/Core/Object/Type.hpp"

#define _WL_OBJECT_NAME_PREFIX(Next) _Object##Next
#define _WL_OBJECT_NAME_TYPE(NameType) _WL_OBJECT_NAME_PREFIX(NameType##Type)

#define _WL_OBJECT_BINDING_METHOD_NAME() _WL_OBJECT_NAME_PREFIX(BindMethods(TypeBuilder& builder))
#define _WL_OBJECT_BINDING_METHOD_PROTOTYPE() void _WL_OBJECT_BINDING_METHOD_NAME()

#define WL_OBJECT(Derived, Base)                                                                    \
private:                                                                                            \
    const inline static ::Wl::Type _WL_OBJECT_NAME_TYPE(Derived) = ::Wl::Type::Register<Derived>(); \
                                                                                                    \
public:                                                                                             \
    inline static ::Wl::Type StaticType()                                                           \
    {                                                                                               \
        return _WL_OBJECT_NAME_TYPE(Derived);                                                       \
    }                                                                                               \
                                                                                                    \
private:                                                                                            \
    friend _WL_OBJECT_BINDING_METHOD_PROTOTYPE();                                                   \
    static _WL_OBJECT_BINDING_METHOD_PROTOTYPE();

namespace Wl
{

    class Object
    {
    };

}// namespace Wl
