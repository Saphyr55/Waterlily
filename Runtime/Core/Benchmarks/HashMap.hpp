#pragma once

#include <iomanip>
#include <iostream>
#include <random>
#include <unordered_map>
#include <vector>

#include "Benchmark.hpp"
#include "Waterlily/Core/Containers/HashMap.hpp"

struct HashMapBenchmarkResult
{
    std::string name = "";
    double insert = -1.0;
    double lookup = -1.0;
    double erase = -1.0;
    double clear = -1.0;
};

template<typename MapType, typename PutFunc, typename GetFunc, typename RemoveFunc, typename ClearFunc>
HashMapBenchmarkResult run_benchmark(const std::string_view name,
                                     std::vector<int32_t>& keys,
                                     MapType& map,
                                     PutFunc put,
                                     GetFunc get,
                                     RemoveFunc remove,
                                     ClearFunc clear)
{
    HashMapBenchmarkResult result = {};
    result.name = name;

    result.insert = Wl::benchmark::start([&]()
    {
        for (auto k: keys)
        {
            put(map, k, k);
        }
    });

    result.lookup = Wl::benchmark::start([&]()
    {
        for (auto k: keys)
        {
            volatile int32_t v = get(map, k);
        }
    });

    result.erase = Wl::benchmark::start([&]()
    {
        for (auto k: keys)
        {
            remove(map, k);
        }
    });

    result.clear = Wl::benchmark::start([&]()
    {
        clear(map);
    });

    return result;
}

inline void print_results_table(const std::vector<HashMapBenchmarkResult>& results)
{
    std::cout << std::setw(25) << "Map" << std::setw(25) << "Insert (s)" << std::setw(25) << "Lookup (s)"
              << std::setw(25) << "Erase (s)" << std::setw(25) << "Clear (s)" << "\n";

    std::cout << std::string(125, '-') << "\n";

    for (const auto& r: results)
    {
        std::cout << std::setw(25) << r.name << std::setw(25) << r.insert << std::setw(25) << r.lookup << std::setw(25)
                  << r.erase << std::setw(25) << r.clear << "\n";
    }
}

inline void run_hash_map_benchmarks()
{
    const auto [N, N_cstr] = NUMBER_AND_QUOTE(10'000'000);
    std::vector<int32_t> keys(N);
    for (size_t i = 0; i < N; i++)
    {
        keys[i] = i;
    }
    std::shuffle(keys.begin(), keys.end(), std::mt19937{std::random_device{}()});

    std::cout << "\n\nBenchmark insertion, lookup, erase, clear on " << N_cstr << " elements\n\n";

    std::vector<HashMapBenchmarkResult> results;
    // STL unordered_map
    {
        std::unordered_map<int32_t, int32_t> stl_unordered_map;
        results.push_back(run_benchmark(
                "std::unordered_map",
                keys,
                stl_unordered_map,
                [](auto& m, int32_t k, int32_t v)
        {
            m[k] = v;
        },
                [](auto& m, int32_t k)
        {
            return m[k];
        },
                [](auto& m, int32_t k)
        {
            m.erase(k);
        },
                [](auto& map)
        {
            map.clear();
        }));
    }

    // Linear Probing HashMap
    {
        Wl::HashMap<int32_t, int32_t> robin_map;
        results.push_back(run_benchmark(
                "Wl::HashMap",
                keys,
                robin_map,
                [](auto& m, int32_t k, int32_t v)
        {
            m.Put(k, v);
        },
                [](auto& m, int32_t k)
        {
            return m.Get(k);
        },
                [](auto& m, int32_t k)
        {
            m.Remove(k);
        },
                [](auto& map)
        {
            map.Clear();
        }));
    }

    print_results_table(results);
}
