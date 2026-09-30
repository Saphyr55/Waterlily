#include "Waterlily/Core/Object/Object.hpp"
#include "Waterlily/Core/Object/MetaTable.hpp"
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

    float FooMethod(int x, float r) const
    {
        return (x + m_y) / r;
    }

    int& GetRefX()
    {
        return m_x;
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
    int m_x = 91;
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
    Variable xVar = MetaTable::VariableOf<decltype(Foo::m_x)>();

    MetaTable::RegisterMember(StaticType(), xVar, "x", OffsetOfX(), xVar.GetSize(), xVar.GetAlign());
    MetaTable::RegisterMember("y", &Foo::m_y);

    MetaTable::RegisterMethod("FooMethod", &Foo::FooMethod);
    MetaTable::RegisterMethod("GetRefX", &Foo::GetRefX);
}

static int before = []()
{
    Foo::BindAll();
    return 1;
}();

TEST_CASE("Object::StaticClass", "[Object]")
{
    SECTION("foo.GetObjectType().GetName() == \"class Foo\"")
    {
        Foo foo;
        REQUIRE(foo.GetObjectType().GetName() == "class Foo");
    }

    SECTION("Wl::MetaTable::GetTypeInfo<Foo>().name == \"class Foo\"")
    {
        REQUIRE(MetaTable::GetTypeInfo<Foo>().name == "class Foo");
    }

    SECTION("Foo::StaticType().GetName() == \"class Foo\"")
    {
        REQUIRE(Foo::StaticType().GetName() == "class Foo");
    }

    SECTION("Foo::StaticType().InheritFrom(Bar::StaticType()) is true")
    {
        REQUIRE(Foo::StaticType().InheritFrom(Bar::StaticType()));
        REQUIRE(Foo::StaticType().InheritFrom(Object::StaticType()));
    }

    SECTION("Bar::StaticType().InheritFrom(Foo::StaticType()) is false")
    {
        REQUIRE_FALSE(Bar::StaticType().InheritFrom(Foo::StaticType()));
        REQUIRE(Bar::StaticType().InheritFrom(Object::StaticType()));
    }
}

TEST_CASE("MetaTable::Method", "[Object]")
{
    StringID fooMethodName = "FooMethod";

    SECTION("Check contains Method")
    {
        REQUIRE(MetaTable::ConstainsMethod(Foo::StaticType(), fooMethodName));
    }

    SECTION("Check method info")
    {
        const MethodInfo& info = MetaTable::GetMethodInfo(Foo::StaticType(), fooMethodName);
        REQUIRE(info.name == fooMethodName);
        REQUIRE(info.owner == Foo::StaticType());
        REQUIRE(info.paramVars.GetSize() == 2);
        REQUIRE(info.paramVars[0] == MetaTable::VariableOf<int>());
        REQUIRE(info.paramVars[1] == MetaTable::VariableOf<float>());
        REQUIRE(info.returnVar == MetaTable::VariableOf<float>());
    }

    SECTION("Check method handle invoke")
    {
        Foo foo;
        const MethodInfo& info = MetaTable::GetMethodInfo(Foo::StaticType(), fooMethodName);
        float x = info.handle.Invoke<float>(foo, 2, 1.0f);
        REQUIRE(x == 2.0f);
    }
}

TEST_CASE("MetaTable::Member", "[Object]")
{
    const MemberInfo& infoX = MetaTable::GetMemberInfo(Foo::StaticType(), "x");
    const MemberInfo& infoY = MetaTable::GetMemberInfo(Foo::StaticType(), "y");

    SECTION("Member x")
    {
        REQUIRE(infoX.name == "x");
        REQUIRE(infoX.offset == Foo::OffsetOfX());
        REQUIRE(infoX.size == sizeof(int));
        REQUIRE(infoX.align == alignof(int));
    }

    SECTION("Member y")
    {
        REQUIRE(infoY.name == "y");
        REQUIRE(infoY.offset == Foo::OffsetOfY());
        REQUIRE(infoY.size == sizeof(const int));
        REQUIRE(infoY.align == alignof(const int));
    }

    SECTION("Can read member with an offset.")
    {
        Foo foo;

        const uint8_t* base = reinterpret_cast<const uint8_t*>(&foo);
        foo.SetX(42);

        const int value = *reinterpret_cast<const int*>(base + infoX.offset);

        REQUIRE(value == 42);
    }
}
