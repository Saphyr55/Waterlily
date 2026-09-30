#include "Waterlily/Core/Object/Object.hpp"
#include "Waterlily/Core/Object/MetaTable.hpp"
#include "Waterlily/Core/Object/Type.hpp"
#include "Waterlily/Core/Object/TypeDescriptor.hpp"
#include "Waterlily/Core/Object/TypeInfo.hpp"
#include "Waterlily/Core/String/Format.hpp"
#include "Waterlily/Core/String/String.hpp"
#include "Waterlily/Core/String/StringID.hpp"

#include <catch2/catch_test_macros.hpp>
#include <memory>

using namespace Wl;

class Bar : public Object
{
    WL_OBJECT(Bar, Object);
};

class Foo : public Bar
{
    WL_OBJECT(Foo, Bar);

public:
    String FooMethod(int x, float r) const
    {
        return Format("%.1f", (x + m_y) / r);
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
    String z;

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

void Bar::_RegisterBindings()
{
}

void Foo::_RegisterBindings()
{
    MetaTable::RegisterMember("x", &Foo::m_x);
    MetaTable::RegisterMember("y", &Foo::m_y);
    MetaTable::RegisterMember("z", &Foo::z);

    MetaTable::RegisterMethod("FooMethod", &Foo::FooMethod);
    MetaTable::RegisterMethod("GetRefX", &Foo::GetRefX);

    MetaTable::RegisterMethod("GetX", &Foo::GetX);
    MetaTable::RegisterMethod("SetX", &Foo::SetX);

    MetaTable::RegisterProperty(Foo::StaticType(), "x", "GetX", "SetX");
}

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

TEST_CASE("MetaTable::Property", "[Object]")
{
    StringID propertyName = "x";

    SECTION("Check contains Property")
    {
        REQUIRE(MetaTable::ConstainsProperty(Foo::StaticType(), propertyName));
    }

    SECTION("Check property info")
    {
        const PropertyInfo& info = MetaTable::GetPropertyInfo(Foo::StaticType(), propertyName);
        REQUIRE(info.name == propertyName);
        REQUIRE(info.getterMethodName == StringID("GetX"));
        REQUIRE(info.setterMethodName == StringID("SetX"));
    }

    SECTION("Check getter invoke")
    {
        Foo foo;
        int value = MetaTable::ReadProperty<int>(foo, propertyName);
        REQUIRE(value == 91);
    }

    SECTION("Check setter invoke")
    {
        Foo foo;
        MetaTable::WriteProperty(foo, propertyName, 42);
        REQUIRE(foo.GetX() == 42);
    }
}

TEST_CASE("MetaTable::Method", "[Object]")
{
    StringID fooMethodName = "FooMethod";
    StringID getRefXName = "GetRefX";

    SECTION("Check contains Method")
    {
        REQUIRE(MetaTable::ConstainsMethod(Foo::StaticType(), fooMethodName));
        REQUIRE(MetaTable::ConstainsMethod(Foo::StaticType(), getRefXName));
    }

    SECTION("Check method info")
    {
        const MethodInfo& info = MetaTable::GetMethodInfo(Foo::StaticType(), fooMethodName);
        REQUIRE(info.name == fooMethodName);
        REQUIRE(info.owner == Foo::StaticType());
        REQUIRE(info.paramVars.GetSize() == 2);
        REQUIRE(info.paramVars[0] == MetaTable::TypeDescriptorOf<int>());
        REQUIRE(info.paramVars[1] == MetaTable::TypeDescriptorOf<float>());
        REQUIRE(info.returnVar == MetaTable::TypeDescriptorOf<String>());
    }

    SECTION("Check method handle return ref")
    {
        Foo foo;
        const MethodInfo& info = MetaTable::GetMethodInfo(foo.GetObjectType(), getRefXName);
        int& x = info.handle.Invoke<int&>(foo);
        REQUIRE(std::addressof(x) == std::addressof(foo.GetRefX()));
    }

    SECTION("Check method handle invoke")
    {
        Foo foo;
        const MethodInfo& info = MetaTable::GetMethodInfo(foo.GetObjectType(), fooMethodName);
        String x = info.handle.Invoke<String>(foo, 2, 1.0f);
        REQUIRE(x == String("2.0"));
    }
}

TEST_CASE("MetaTable::Member", "[Object]")
{

    SECTION("Member x")
    {
        const MemberInfo& info = MetaTable::GetMemberInfo(Foo::StaticType(), "x");

        REQUIRE(info.name == "x");
        REQUIRE(info.offset == Foo::OffsetOfX());
        REQUIRE(info.size == sizeof(int));
        REQUIRE(info.align == alignof(int));
    }

    SECTION("Member y")
    {
        const MemberInfo& info = MetaTable::GetMemberInfo(Foo::StaticType(), "y");

        REQUIRE(info.name == "y");
        REQUIRE(info.offset == Foo::OffsetOfY());
        REQUIRE(info.size == sizeof(int));
        REQUIRE(info.align == alignof(int));
    }

    SECTION("Member z")
    {
        const MemberInfo& info = MetaTable::GetMemberInfo(Foo::StaticType(), "z");

        REQUIRE(info.name == "z");
        REQUIRE(info.offset == Foo::OffsetOfZ());
        REQUIRE(info.size == sizeof(String));
        REQUIRE(info.align == alignof(String));
    }

    SECTION("Can read member with an offset.")
    {
        Foo foo;
        foo.SetX(42);
        REQUIRE(MetaTable::ReadMember<int>(foo, "x") == 42);
    }
}
