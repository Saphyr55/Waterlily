#include "Waterlily/Core/Object/Object.hpp"
#include "Waterlily/Core/Object/TypeInfo.hpp"

#include <catch2/catch_test_macros.hpp>

using namespace Wl;

class Foo : public Object
{
    WL_OBJECT(Foo, Object);

public:
    int BarMethod(int x)
    {
        return x + GetX();
    }

    int GetX() const
    {
        return m_x;
    }

    void SetX(int x)
    {
        m_x = x;
    }

private:
    int m_x;
    int m_y;

public:
    int z;
};

void Foo::_ObjectBindMethods(TypeBuilder& builder)
{
    // builder.AddProperty("X", &Foo::m_x, &Foo::GetX, &Foo::SetX);
    // builder.AddProperty("Y", &Foo::m_y);
    // builder.AddProperty("z", &Foo::z);
    // builder.AddMethod("BarMethod", &Foo::BarMethod);
}

TEST_CASE("Object", "[Object]")
{
    SECTION("Wl::Type::GetTypeInfo<Foo>().Name == \"class Foo\"")
    {
        REQUIRE(Type::GetTypeInfo<Foo>().Name == "class Foo");
    }

    SECTION("Foo::StaticType().GetTypeInfo().Name == \"class Foo\"")
    {
        REQUIRE(Foo::StaticType().GetTypeInfo().Name == "class Foo");
    }
}
