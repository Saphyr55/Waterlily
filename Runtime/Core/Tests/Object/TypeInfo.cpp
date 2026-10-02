#include "Waterlily/Core/Object/TypeInfo.hpp"

#include <catch2/catch_test_macros.hpp>

namespace Namespace1::Namespace2
{
    class AClass
    {
        int x;
        int* a;
    };
}// namespace Namespace1::Namespace2

TEST_CASE("TypeName", "[TypeName]")
{

    SECTION("Wl::TypeName<void>() == \"void\"")
    {
        REQUIRE(Wl::TypeName<void>() == "void");
    }

    SECTION("Wl::TypeName<Namespace1::Namespace2::AClass>() == \"class Namespace1::Namespace2::AClass\"")
    {
        REQUIRE(Wl::TypeName<Namespace1::Namespace2::AClass>() == "class Namespace1::Namespace2::AClass");
    }

    SECTION("Nested Wl::TypeName<AClass>() == \"class Namespace1::Namespace2::AClass\"")
    {
        using namespace Namespace1::Namespace2;

        REQUIRE(Wl::TypeName<AClass>() == "class Namespace1::Namespace2::AClass");
    }
}
