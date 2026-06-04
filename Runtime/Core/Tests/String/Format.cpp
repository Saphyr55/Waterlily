#include "Waterlily/Core/String/Format.hpp"
#include <catch2/catch_test_macros.hpp>

using namespace Wl;

TEST_CASE("Wl::Format - Basic formatting", "[format]")
{
    SECTION("Simple string formatting")
    {
        String result = Wl::Format("Hello %s", "World");
        REQUIRE(result == "Hello World");
    }

    SECTION("Integer formatting")
    {
        String result = Wl::Format("Number: %d", 42);
        REQUIRE(result == "Number: 42");
    }

    SECTION("Multiple arguments")
    {
        String result = Wl::Format("%s %d %.2f", "Test", 123, 45.67f);
        REQUIRE(result == "Test 123 45.67");
    }

    SECTION("Float formatting")
    {
        String result = Wl::Format("Pi: %.2f", 3.14159f);
        REQUIRE(result == "Pi: 3.14");
    }

    SECTION("Character formatting")
    {
        String result = Wl::Format("Char: %c", 'A');
        REQUIRE(result == "Char: A");
    }
}
