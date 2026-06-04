#pragma once

namespace Wl
{

    template<typename T>
    inline auto Map(T* begin, T* end, auto&& functor) -> void
    {
        for (auto it = begin; it != end; it++)
        {
            functor(*it);
        }
    }

    template<typename T>
    inline auto Reduce(T* begin, T* end, auto initiale_value, auto&& functor) -> auto
    {
        auto value = initiale_value;
        for (auto it = begin; it != end; it++)
        {
            value = functor(value, *it);
        }
        return value;
    }

}// namespace Wl