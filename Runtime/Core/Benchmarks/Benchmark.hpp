#pragma once

#include <chrono>

#include "Waterlily/Core/Defines.hpp"

#define NUMBER_AND_QUOTE(x) std::make_tuple<usize, const char*>(x, #x)

namespace Wl::Benchmark
{

    template<typename Func>
    inline double Start(Func func, usize repeat = 3)
    {
        using namespace std::chrono;
        double total = 0.0;
        for (usize i = 0; i < repeat; i++)
        {
            auto start = high_resolution_clock::now();
            func();
            auto end = high_resolution_clock::now();
            total += duration_cast<duration<double>>(end - start).count();
        }
        return total / repeat;
    }

}// namespace Wl::Benchmark
