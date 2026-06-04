#pragma once

#include <chrono>

#include "Waterlily/Core/Defines.hpp"

#define NUMBER_AND_QUOTE(x) std::make_tuple<size_t, const char*>(x, #x)

namespace Wl::benchmark
{

    template<typename Func>
    inline double start(Func func, size_t repeat = 3)
    {
        using namespace std::chrono;
        double total = 0.0;
        for (size_t i = 0; i < repeat; i++)
        {
            auto start = high_resolution_clock::now();
            func();
            auto end = high_resolution_clock::now();
            total += duration_cast<duration<double>>(end - start).count();
        }
        return total / repeat;
    }

}// namespace Wl::benchmark
