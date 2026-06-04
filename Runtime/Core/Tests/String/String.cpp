#include <catch2/catch_all.hpp>

#include "Waterlily/Core/String/String.hpp"

#include <cstring>

using namespace Wl;

TEST_CASE("StringBase - Construction", "[string]")
{
    SECTION("Default construction")
    {
        String str;
        REQUIRE(str.GetSize() == 0);
        REQUIRE(str.GetCapacity() >= 0);
        REQUIRE(strcmp(str.data(), "") == 0);
    }

    SECTION("Construction with capacity")
    {
        String str(100);
        REQUIRE(str.GetSize() == 0);
        REQUIRE(str.GetCapacity() >= 100);
        REQUIRE(strcmp(str.data(), "") == 0);
    }

    SECTION("Construction from C-string")
    {
        String str("Hello");
        REQUIRE(str.GetSize() == 5);
        REQUIRE(strcmp(str.data(), "Hello") == 0);
    }

    SECTION("Copy construction")
    {
        String original("Hello");
        String copy(original);

        REQUIRE(copy.GetSize() == original.GetSize());
        REQUIRE(strcmp(copy.data(), original.data()) == 0);
    }
}

TEST_CASE("StringBase - Assignment", "[string]")
{
    SECTION("Copy assignment")
    {
        String original("Hello");
        String assigned;

        assigned = original;

        REQUIRE(assigned.GetSize() == original.GetSize());
        REQUIRE(strcmp(assigned.data(), original.data()) == 0);
    }

    SECTION("Self assignment")
    {
        String str("Hello");
        str = str;// Should not cause issues

        REQUIRE(strcmp(str.data(), "Hello") == 0);
    }
}

TEST_CASE("StringBase - Append operations", "[string]")
{
    SECTION("Append C-string")
    {
        String str("Hello");
        str.Append(" World");

        REQUIRE(str.GetSize() == 11);
        REQUIRE(strcmp(str.GetData(), "Hello World") == 0);
    }

    SECTION("Append character")
    {
        String str("Hello");
        str.Append('!');

        REQUIRE(str.GetSize() == 6);
        REQUIRE(strcmp(str.GetData(), "Hello!") == 0);
    }

    SECTION("Append another string")
    {
        String str1("Hello");
        String str2(" World");

        str1.Append(str2);

        REQUIRE(str1.GetSize() == 11);
        REQUIRE(strcmp(str1.GetData(), "Hello World") == 0);
    }

    SECTION("Multiple appends")
    {
        String str;
        str.Append("Hello").Append(' ').Append("World").Append('!');

        REQUIRE(str.GetSize() == 12);
        REQUIRE(strcmp(str.GetData(), "Hello World!") == 0);
    }
}

TEST_CASE("StringBase - Operator += overloads", "[string]")
{
    SECTION("+= with C-string")
    {
        String str("Hello");
        str += " World";

        REQUIRE(strcmp(str.GetData(), "Hello World") == 0);
    }

    SECTION("+= with character")
    {
        String str("Hello");
        str += '!';

        REQUIRE(strcmp(str.GetData(), "Hello!") == 0);
    }

    SECTION("+= with another string")
    {
        String str1("Hello");
        String str2(" World");

        str1 += str2;

        REQUIRE(strcmp(str1.GetData(), "Hello World") == 0);
    }
}

TEST_CASE("StringBase - Comparison operations", "[string]")
{
    SECTION("Equality comparison")
    {
        String str1("Hello");
        String str2("Hello");
        String str3("World");

        REQUIRE(str1 == str2);
        REQUIRE_FALSE(str1 == str3);
    }

    SECTION("Comparison with different lengths")
    {
        String str1("Hello");
        String str2("Hello World");

        REQUIRE_FALSE(str1 == str2);
    }
}

TEST_CASE("StringBase - Capacity management", "[string]")
{
    SECTION("Resize operation")
    {
        String str("Hello");
        str.Resize(10);

        REQUIRE(str.GetSize() == 10);
        REQUIRE(str.GetData()[10] == '\0');// Null-terminated
    }

    SECTION("Reserve operation")
    {
        String str;
        size_t initialCapacity = str.GetCapacity();

        str.Reserve(100);

        REQUIRE(str.GetCapacity() >= 100);
        REQUIRE(str.GetSize() == 0);// size unchanged
        REQUIRE(strcmp(str.GetData(), "") == 0);
    }

    SECTION("Clear operation")
    {
        String str("Hello World");
        str.Clear();

        REQUIRE(str.GetSize() == 0);
        REQUIRE(strcmp(str.GetData(), "") == 0);
    }

    SECTION("Empty check")
    {
        String emptyString;
        String nonEmptyString("Hello");

        REQUIRE_FALSE(nonEmptyString.IsEmpty());
    }
}

TEST_CASE("WString - Specific functionality", "[string][wstring]")
{
    SECTION("WString construction")
    {
        WString wstr(L"Hello");
        REQUIRE(wstr.GetSize() == 5);
        REQUIRE(wcscmp(wstr.GetData(), L"Hello") == 0);
    }

    SECTION("WString append")
    {
        WString wstr(L"Hello");
        wstr.Append(L" World");

        REQUIRE(wstr.GetSize() == 11);
        REQUIRE(wcscmp(wstr.GetData(), L"Hello World") == 0);
    }
}

TEST_CASE("String - Edge cases", "[string]")
{
    SECTION("Empty C-string construction")
    {
        String str("");
        REQUIRE(str.GetSize() == 0);
        REQUIRE(strcmp(str.GetData(), "") == 0);
    }

    SECTION("Append empty string")
    {
        String str("Hello");
        String empty;

        str.Append(empty);
        REQUIRE(strcmp(str.GetData(), "Hello") == 0);
    }

    SECTION("Append to empty string")
    {
        String str;
        str.Append("Hello");

        REQUIRE(strcmp(str.GetData(), "Hello") == 0);
    }

    SECTION("Multiple null terminators handling")
    {
        // This tests that the internal buffer management correctly handles
        // the removal and re-addition of null terminators
        String str;
        str.Append('H').Append('e').Append('l').Append('l').Append('o');

        REQUIRE(strcmp(str.GetData(), "Hello") == 0);
    }
}

TEST_CASE("String - Output stream operator", "[string][ostream]")
{
    SECTION("Stream output")
    {
        String str("Hello World");
        std::ostringstream oss;

        oss << str;

        REQUIRE(oss.str() == "Hello World");
    }

    SECTION("Empty string stream output")
    {
        String str;
        std::ostringstream oss;

        oss << str;

        REQUIRE(oss.str() == "");
    }
}
