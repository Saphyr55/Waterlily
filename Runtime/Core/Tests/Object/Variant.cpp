#include "Waterlily/Core/Object/Variant.hpp"
#include "Waterlily/Core/Object/MetaTable.hpp"
#include "Waterlily/Core/Object/Primitive.hpp"
#include "Waterlily/Core/Object/Type.hpp"
#include "Waterlily/Core/Object/TypeInfo.hpp"

#include <catch2/catch_test_macros.hpp>

using namespace Wl;

TEST_CASE("Variant Inline storage has expected size", "[Variant]")
{
    REQUIRE(Variant::InlineSize == 16);
    REQUIRE(Variant::InlineAlignment == alignof(std::max_align_t));
}

TEST_CASE("Variant Inline storage is aligned", "[Variant]")
{
    Variant variant(Int32Type);
    uintptr address = reinterpret_cast<uintptr>(variant.GetData());
    REQUIRE(address % Variant::InlineAlignment == 0);
}

TEST_CASE("Variant Primitive is stored inline", "[Variant]")
{
    Type type = Int32Type;

    Variant variant(type);

    REQUIRE(variant.GetType() == type);
    REQUIRE(variant.GetTypeInfo().kind == TypeInfo::Kind::Primitive);
    REQUIRE(variant.IsInline());
    REQUIRE(variant.GetData() != nullptr);
}


TEST_CASE("Variant Non primitive is stored on heap", "[Variant]")
{
    Type type = MetaTable::TypeOf<String>();
    Variant variant(type);

    REQUIRE(variant.GetType() == type);
    REQUIRE(variant.GetTypeInfo().kind == TypeInfo::Kind::String);
    REQUIRE_FALSE(variant.IsInline());
    REQUIRE(variant.GetData() != nullptr);
}

TEST_CASE("Variant Default constructs primitive", "[Variant]")
{
    Variant variant(Int32Type);

    int32* value = static_cast<int32*>(variant.GetData());

    REQUIRE(value != nullptr);
    REQUIRE(*value == 0);
}

TEST_CASE("Variant Assign primitive", "[Variant]")
{
    Variant variant(Int32Type);

    variant.Assign(42);

    int32* value = static_cast<int32*>(variant.GetData());

    REQUIRE(variant.GetType() == MetaTable::TypeOf<int>());
    REQUIRE(variant.IsInline());
    REQUIRE(*value == 42);
}

TEST_CASE("Variant Assign String", "[Variant]")
{
    Variant variant(MetaTable::TypeOf<String>());
    String expected = "Hello Waterlily";

    variant.Assign(expected);

    String* value = static_cast<String*>(variant.GetData());

    REQUIRE_FALSE(variant.IsInline());
    REQUIRE(*value == expected);
}

TEST_CASE("Variant String can be modified through GetData", "[Variant]")
{
    Variant variant(MetaTable::TypeOf<String>());

    variant.Assign(String("Hello"));

    String* value = static_cast<String*>(variant.GetData());
    *value = "World";

    REQUIRE(*static_cast<const String*>(variant.GetData()) == "World");
}

TEST_CASE("Variant Assign String from temporary", "[Variant]")
{
    Variant variant(MetaTable::TypeOf<String>());

    variant.Assign(String("Hello Waterlily"));

    String* value = variant.Get<String>();

    REQUIRE_FALSE(variant.IsInline());
    REQUIRE(*value == "Hello Waterlily");
}

