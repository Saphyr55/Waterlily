#pragma once

#include "Benchmark.hpp"
#include "Waterlily/Core/Containers/Array.hpp"

#include <iomanip>
#include <iostream>
#include <random>
#include <vector>

struct Object
{
    int32_t id;
    double value;

    Object(int32_t p_id = 0, double p_value = 0.0)
        : id(p_id)
        , value(p_value)
    {
    }
};

struct ArrayBenchmarkResult
{
    std::string name = "";
    double insert = -1.0;
    double random_access = -1.0;
    double erase = -1.0;
    double clear = -1.0;
};

template<typename ArrayType, typename InsertFunc, typename AccessFunc, typename RemoveFunc, typename ClearFunc>
ArrayBenchmarkResult run_array_benchmark(const std::string_view name,
                                         std::vector<int32_t>& keys,
                                         ArrayType& arr,
                                         InsertFunc insert,
                                         AccessFunc access,
                                         RemoveFunc remove,
                                         ClearFunc clear)
{
    ArrayBenchmarkResult result;
    result.name = name;

    result.insert = Wl::benchmark::start([&]()
    {
        for (auto& k: keys)
        {
            insert(arr, k);
        }
    });

    result.random_access = Wl::benchmark::start([&]()
    {
        for (auto& k: keys)
        {
            volatile Object v = access(arr, k % arr.size());
        }
    });

    result.erase = Wl::benchmark::start([&]()
    {
        for (auto& k: keys)
        {
            remove(arr);
        }
    });

    result.clear = Wl::benchmark::start([&]()
    {
        clear(arr);
    });

    return result;
}

inline void print_array_results_table(const std::vector<ArrayBenchmarkResult>& results)
{
    std::cout << std::setw(25) << "Array" << std::setw(25) << "Insert (s)" << std::setw(25) << "Access (s)"
              << std::setw(25) << "Erase (s)" << std::setw(25) << "Clear (s)" << "\n";

    std::cout << std::string(125, '-') << "\n";

    for (const auto& r: results)
    {
        std::cout << std::setw(25) << r.name << std::setw(25) << r.insert << std::setw(25) << r.random_access
                  << std::setw(25) << r.erase << std::setw(25) << r.clear << "\n";
    }
}

inline void run_array_benchmarks()
{
    const auto [N, N_cstr] = NUMBER_AND_QUOTE(1000000);

    std::vector<int32_t> keys(N);
    for (size_t i = 0; i < N; i++)
    {
        keys[i] = i;
    }

    std::shuffle(keys.begin(), keys.end(), std::mt19937{std::random_device{}()});

    std::cout << "\n\nBenchmark insertion, random access, erase, clear on " << N_cstr << " elements\n\n";

    std::vector<ArrayBenchmarkResult> results;

    // std::vector
    {
        std::vector<Object> v;

        results.push_back(run_array_benchmark(
                "std::vector",
                keys,
                v,
                [](std::vector<Object>& v, Object k)
        {
            v.push_back(k);
        },
                [](std::vector<Object>& v, size_t idx)
        {
            return v[idx];
        },
                [](std::vector<Object>& v)
        {
            v.pop_back();
        },
                [](std::vector<Object>& v)
        {
            v.clear();
        }));
    }

    // Wl::Array
    {
        Wl::Array<Object> arr;

        results.push_back(run_array_benchmark(
                "Wl::Array",
                keys,
                arr,
                [](Wl::Array<Object>& a, Object k)
        {
            a.Append(k);
        },
                [](Wl::Array<Object>& a, size_t idx)
        {
            return a[idx];
        },
                [](Wl::Array<Object>& a)
        {
            a.Pop();
        },
                [](Wl::Array<Object>& v)
        {
            v.Clear();
        }));
    }

    print_array_results_table(results);
}
