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

    template<typename BeginIterator, typename EndIterator, typename OutputIterator, typename Func>
    inline auto Transform(BeginIterator begin, EndIterator end, OutputIterator output, Func&& func)
    {
        while (begin != end)
        {
            *output = func(*begin);

            begin++;
            output++;
        }
    }

    template<typename BeginIterator, typename EndIterator, typename T>
    inline auto Constains(BeginIterator begin, EndIterator end, const T& value) -> bool
    {
        while (begin != end)
        {
            if (*begin == value)
            {
                return true;
            }
            begin++;
        }
        return false;
    }

}// namespace Wl