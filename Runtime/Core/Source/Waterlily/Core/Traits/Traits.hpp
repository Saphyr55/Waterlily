#pragma once

#include "TypeList.hpp"

namespace Wl
{

    template<typename T>
    struct RemoveAllPointers
    {
        using Type = T;
    };

    template<typename T>
    struct RemoveAllPointers<T*>
    {
        using Type = RemoveAllPointers<T>::Type;
    };

    template<typename T>
    using RemoveAllPointersType = RemoveAllPointers<T>::Type;

    template<typename T>
    struct MethodTraits;

    template<typename Type, typename ReturnType, typename... ParameterTypes>
    struct MethodTraits<ReturnType (Type::*)(ParameterTypes...)>
    {
        using Method = ReturnType (Type::*)(ParameterTypes...);
        using Object = Type;
        using Return = ReturnType;
        using Params = TypeList<ParameterTypes...>;
    };

    template<typename Type, typename ReturnType, typename... ParameterTypes>
    struct MethodTraits<ReturnType (Type::*)(ParameterTypes...) const>
        : MethodTraits<ReturnType (Type::*)(ParameterTypes...)>
    {
        using Method = ReturnType (Type::*)(ParameterTypes...) const;
    };

}// namespace Wl