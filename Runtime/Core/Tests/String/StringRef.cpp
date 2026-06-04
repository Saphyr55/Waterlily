#include "Waterlily/Core/String/StringRef.hpp"
#include "Waterlily/Core/String/String.hpp"

#include <catch2/catch_test_macros.hpp>

using namespace Wl;

TEST_CASE("StringRefBase - Basic functionality", "[StringRefBase]")
{
    SECTION("Default construction")
    {
        StringRef strRef;
        REQUIRE(strcmp(strRef.GetData(), "") == 0);
        REQUIRE(strRef.IsEmpty());
        REQUIRE(strRef.GetSize() == 0);
    }

    SECTION("Construction from C-string")
    {
        const char* testCstr = "Hello";
        StringRef strRef(testCstr);
        REQUIRE(strRef.GetData() == testCstr);
        REQUIRE(strRef.GetSize() == 5);
        REQUIRE_FALSE(strRef.IsEmpty());
    }

    SECTION("Construction from String")
    {
        String str("Test");
        StringRef GetRef(str);
        REQUIRE(GetRef.GetData() == str.GetData());
        REQUIRE(GetRef.GetSize() == 4);
        REQUIRE(GetRef == "Test");
    }

    SECTION("Copy construction and assignment")
    {
        StringRef original("Original");
        StringRef copy(original);
        StringRef assigned;
        assigned = original;

        REQUIRE(copy == original);
        REQUIRE(assigned == original);
    }

    SECTION("Element access")
    {
        StringRef strRef("ABC");
        REQUIRE(strRef[0] == 'A');
        REQUIRE(strRef[1] == 'B');
        REQUIRE(strRef[2] == 'C');
    }

    SECTION("Comparison operators")
    {
        StringRef ref1("Hello");
        StringRef ref2("Hello");
        StringRef ref3("World");

        REQUIRE(ref1 == ref2);
        REQUIRE_FALSE(ref1 == ref3);
        REQUIRE(ref1 == "Hello");
        REQUIRE_FALSE(ref1 == "World");
    }

    SECTION("Implicit conversion to const char*")
    {
        StringRef strRef("Test");
        const char* cstr = strRef;
        REQUIRE(StringCompare(cstr, "Test") == 0);
    }
}

TEST_CASE("StringRefBase - Edge cases", "[StringRefBase]")
{
    SECTION("Empty string")
    {
        Wl::StringRef strRef("");
        REQUIRE(strRef.IsEmpty());
        REQUIRE(strRef.GetSize() == 0);
        REQUIRE(strRef.GetData()[0] == '\0');
    }
}

TEST_CASE("StringRefBase - Output stream", "[StringRefBase][ostream]")
{
    SECTION("Stream output")
    {
        Wl::StringRef strRef("StreamTest");
        std::ostringstream oss;
        oss << strRef;
        REQUIRE(oss.str() == "StreamTest");
    }
}
