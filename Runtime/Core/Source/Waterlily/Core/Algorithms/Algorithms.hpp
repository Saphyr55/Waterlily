#pragma once

namespace Wl
{

    template<typename BeginIterator, typename EndIterator, typename OutputIterator, typename Func>
    void Transform(BeginIterator begin, EndIterator end, OutputIterator output, Func&& func)
    {
        while (begin != end)
        {
            *output = func(*begin);

            begin++;
            output++;
        }
    }

    template<typename BeginIterator, typename EndIterator, typename T>
    bool Constains(BeginIterator begin, EndIterator end, const T& value)
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
