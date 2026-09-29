#pragma once

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


}// namespace Wl