#include <catch2/catch_test_macros.hpp>

#include "Waterlily/Core/Containers/Option.hpp"

using namespace Wl;

TEST_CASE("Option basic construction.", "[Option]")
{
    Option<int> none = Option<int>::None();
    REQUIRE_FALSE(none.HasValue());

    Option<int> some = Option<int>::Some(42);
    REQUIRE(some.HasValue());
    REQUIRE(some.Unwrap() == 42);
    REQUIRE(*some == 42);
}

TEST_CASE("Option unwrapOr works.", "[Option]")
{
    Option<int> none = Option<int>::None();
    REQUIRE(none.UnwrapOr(99) == 99);

    Option<int> some = Option<int>::Some(5);
    REQUIRE(some.UnwrapOr(99) == 5);
}

TEST_CASE("Option copy and assignment.", "[Option]")
{
    Option<int> a = Option<int>::Some(7);
    Option<int> b = a;// copy constructor
    REQUIRE(b.HasValue());
    REQUIRE(b.Unwrap() == 7);

    Option<int> c;
    c = a;// copy assignment
    REQUIRE(c.HasValue());
    REQUIRE(c.Unwrap() == 7);

    Option<int> none = Option<int>::None();
    c = none;
    REQUIRE_FALSE(c.HasValue());
}

TEST_CASE("Option with non-trivial type.", "[Option]")
{
    struct A
    {
        int x;
        A(int v)
            : x(v)
        {
        }
    };

    Option<A> o1 = Option<A>::Some(A(123));
    REQUIRE(o1.HasValue());
    REQUIRE(o1.Unwrap().x == 123);

    Option<A> o2 = o1;
    REQUIRE(o2.HasValue());
    REQUIRE(o2.Unwrap().x == 123);
}
