#include <catch2/catch_test_macros.hpp>

#include "Waterlily/Core/Defines.hpp"
#include "Waterlily/Core/Function/Function.hpp"

using namespace Wl;

int32 add(int32 a, int32 b)
{
    return a + b;
}

struct Multiplier
{
    int32 factor;
    int32 operator()(int32 x) const
    {
        return x * factor;
    }
};

TEST_CASE("FunctionRef default constructed", "[FunctionRef]")
{
    FunctionRef<int32(int32, int32)> f;
    REQUIRE_FALSE(f);// should be empty
}

TEST_CASE("FunctionRef with lambda", "[FunctionRef]")
{
    auto lambda = [](auto x, auto y) -> auto
    {
        return x + y;
    };
    
    FunctionRef<int32(int32, int32)> f(lambda);

    REQUIRE(f);
    REQUIRE(f(2, 3) == 5);
}

TEST_CASE("FunctionRef with free function", "[FunctionRef]")
{
    FunctionRef<int32(int32, int32)> f(add);

    REQUIRE(f);
    REQUIRE(f(10, 5) == 15);
}

TEST_CASE("FunctionRef with functor", "[FunctionRef]")
{
    Multiplier mul{3};
    FunctionRef<int32(int32)> f(mul);

    REQUIRE(f);
    REQUIRE(f(4) == 12);
}

TEST_CASE("FunctionRef copy and assignment", "[FunctionRef]")
{
    auto lambda = [](int32 x)
    {
        return x * x;
    };

    FunctionRef<int32(int32)> f1(lambda);
    REQUIRE(f1(5) == 25);

    FunctionRef<int32(int32)> f2 = f1;// copy constructor
    REQUIRE(f2(6) == 36);

    FunctionRef<int32(int32)> f3;
    f3 = f1;// copy assignment
    REQUIRE(f3(7) == 49);
}
