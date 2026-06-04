#include <catch2/catch_test_macros.hpp>

#include "Waterlily/Core/Defines.hpp"
#include "Waterlily/Core/Function/FunctionRef.hpp"

using namespace Wl;

int32_t add(int32_t a, int32_t b)
{
    return a + b;
}

struct Multiplier
{
    int32_t factor;
    int32_t operator()(int32_t x) const
    {
        return x * factor;
    }
};

TEST_CASE("FunctionRef default constructed", "[FunctionRef]")
{
    FunctionRef<int32_t(int32_t, int32_t)> f;
    REQUIRE_FALSE(f);// should be empty
}

TEST_CASE("FunctionRef with lambda", "[FunctionRef]")
{
    auto lambda = [](int32_t x, int32_t y) -> int32_t
    {
        return x + y;
    };
    FunctionRef<int32_t(int32_t, int32_t)> f(lambda);

    REQUIRE(f);
    REQUIRE(f(2, 3) == 5);
}

TEST_CASE("FunctionRef with free function", "[FunctionRef]")
{
    FunctionRef<int32_t(int32_t, int32_t)> f(add);

    REQUIRE(f);
    REQUIRE(f(10, 5) == 15);
}

TEST_CASE("FunctionRef with functor", "[FunctionRef]")
{
    Multiplier mul{3};
    FunctionRef<int32_t(int32_t)> f(mul);

    REQUIRE(f);
    REQUIRE(f(4) == 12);
}

TEST_CASE("FunctionRef copy and assignment", "[FunctionRef]")
{
    auto lambda = [](int32_t x)
    {
        return x * x;
    };
    FunctionRef<int32_t(int32_t)> f1(lambda);
    REQUIRE(f1(5) == 25);

    FunctionRef<int32_t(int32_t)> f2 = f1;// copy constructor
    REQUIRE(f2(6) == 36);

    FunctionRef<int32_t(int32_t)> f3;
    f3 = f1;// copy assignment
    REQUIRE(f3(7) == 49);
}
