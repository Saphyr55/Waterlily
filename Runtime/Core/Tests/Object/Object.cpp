#include "Waterlily/Core/Object/Object.hpp"
#include "Waterlily/Core/Object/Property.hpp"
#include "Waterlily/Core/Object/Type.hpp"
#include "Waterlily/Core/Object/TypeInfo.hpp"
#include "Waterlily/Core/Object/Variable.hpp"

#include <catch2/catch_test_macros.hpp>

using namespace Wl;

class Bar : public Object
{
    WL_OBJECT(Bar, Object);
};

class Foo : public Bar
{
    WL_OBJECT(Foo, Bar);

public:
    static void BindAll()
    {
        _ObjectRegisterBindings();
    }

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
    const int m_y = 0;

public:
    int z;

public:
    static size_t OffsetOfX()
    {
        return offsetof(Foo, m_x);
    }

    static size_t OffsetOfY()
    {
        return offsetof(Foo, m_y);
    }

    static size_t OffsetOfZ()
    {
        return offsetof(Foo, z);
    }
};

void Foo::_ObjectRegisterBindings()
{
    Variable xVar = Variable::Of<decltype(Foo::m_x)>();

    Property::Register(StaticType(), xVar, "x", OffsetOfX(), xVar.GetSize(), xVar.GetAlign());
    Property::Register("y", &Foo::m_y);
}

TEST_CASE("Object::StaticClass", "[Object]")
{
    SECTION("foo.GetObjectType().GetName() == \"class Foo\"")
    {
        Foo foo;
        REQUIRE(foo.GetObjectType().GetName() == "class Foo");
    }

    SECTION("Wl::Type::GetTypeInfo<Foo>().name == \"class Foo\"")
    {
        REQUIRE(Type::GetTypeInfo<Foo>().name == "class Foo");
    }

    SECTION("Foo::StaticType().GetName() == \"class Foo\"")
    {
        REQUIRE(Foo::StaticType().GetName() == "class Foo");
    }

    SECTION("Foo::StaticType().InheritFrom(Bar::StaticType()) == true")
    {
        REQUIRE(Foo::StaticType().InheritFrom(Bar::StaticType()));
        REQUIRE(Foo::StaticType().InheritFrom(Object::StaticType()));
    }

    SECTION("Foo::StaticType().InheritFrom(Bar::StaticType()) == true")
    {
        REQUIRE_FALSE(Bar::StaticType().InheritFrom(Foo::StaticType()));
        REQUIRE(Bar::StaticType().InheritFrom(Object::StaticType()));
    }

}

TEST_CASE("Object::Properties", "[Object]")
{
    Foo::BindAll();

    const PropertyInfo& infoX = Property::GetPropertyInfo(Foo::StaticType(), "x");
    const PropertyInfo& infoY = Property::GetPropertyInfo(Foo::StaticType(), "y");

    SECTION("Property x")
    {
        REQUIRE(infoX.name == "x");
        REQUIRE(infoX.offset == Foo::OffsetOfX());
        REQUIRE(infoX.size == sizeof(int));
        REQUIRE(infoX.align == alignof(int));
    }

    SECTION("Property y")
    {
        REQUIRE(infoY.name == "y");
        REQUIRE(infoY.offset == Foo::OffsetOfY());
        REQUIRE(infoY.size == sizeof(const int));
        REQUIRE(infoY.align == alignof(const int));
    }

    SECTION("Can read with offset.")
    {
        Foo foo;

        const uint8_t* base = reinterpret_cast<const uint8_t*>(&foo);
        foo.SetX(42);

        const int value = *reinterpret_cast<const int*>(base + infoX.offset);

        REQUIRE(value == 42);
    }
}
