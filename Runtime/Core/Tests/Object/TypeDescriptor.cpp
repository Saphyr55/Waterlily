#include "Waterlily/Core/Object/TypeDescriptor.hpp"
#include "Waterlily/Core/Object/MetaTable.hpp"
#include "Waterlily/Core/Object/TypeInfo.hpp"

#include <catch2/catch_test_macros.hpp>
#include <iostream>

using namespace Wl;

template<typename FirstParamaterType, typename... ParamaterTypes>
void PrintParams()
{
    TypeDescriptor typeDesc = MetaTable::TypeDescriptorOf<FirstParamaterType>();
    std::cout << "\t" << typeDesc.GetTypeName() << "\n";
    if constexpr (sizeof...(ParamaterTypes) != 0)
    {
        PrintParams<ParamaterTypes...>();
    }
}

template<typename ReturnType, typename... ParamaterTypes>
void PrintFunction(ReturnType (*)(ParamaterTypes...))
{
    TypeDescriptor returntypeDesc = MetaTable::TypeDescriptorOf<ReturnType>();
    std::cout << "Return Type " << returntypeDesc.GetTypeName() << "\n";
    std::cout << "Params[\n";
    PrintParams<ParamaterTypes...>();
    std::cout << "]\n";
}

TEST_CASE("TypeDescriptor equality", "[TypeDescriptor]")
{
    Type intType = MetaTable::TypeOf<int>();
    Type floatType = MetaTable::TypeOf<float>();

    SECTION("Default typeDescs are equal")
    {
        TypeDescriptor a;
        TypeDescriptor b;
        REQUIRE(a == b);
        REQUIRE_FALSE(a != b);
    }

    SECTION("Same type is equal")
    {
        TypeDescriptor a(intType);
        TypeDescriptor b(intType);
        REQUIRE(a == b);
        REQUIRE_FALSE(a != b);
    }

    SECTION("Different type is not equal")
    {
        TypeDescriptor a(intType);
        TypeDescriptor b(floatType);
        REQUIRE_FALSE(a == b);
        REQUIRE(a != b);
    }
}

TEST_CASE("TypeDescriptor complex state", "[TypeDescriptor]")
{
    TypeDescriptor typeDesc(MetaTable::TypeOf<int>());
    REQUIRE(typeDesc.IsPlain());

    typeDesc.SetConst();
    typeDesc.AddPointer();
    typeDesc.SetArraySize(16);

    REQUIRE_FALSE(typeDesc.IsPlain());
    REQUIRE(typeDesc.GetUnderlyingType() == MetaTable::TypeOf<int>());
    REQUIRE(typeDesc.IsConst());
    REQUIRE(typeDesc.IsPointer());
    REQUIRE(typeDesc.GetPointerCount() == 1);
    REQUIRE(typeDesc.IsArray());
    REQUIRE(typeDesc.GetArraySize() == 16);
}

TEST_CASE("TypeDescriptor can be used as HashMap key", "[TypeDescriptor]")
{
    HashMap<TypeDescriptor, int> values;

    TypeDescriptor inttypeDesc(MetaTable::TypeOf<int>());
    inttypeDesc.SetConst();

    TypeDescriptor sametypeDesc(MetaTable::TypeOf<int>());
    sametypeDesc.SetConst();

    values.Emplace(inttypeDesc, 42);

    REQUIRE(values.Contains(sametypeDesc));
    REQUIRE(values.Get(sametypeDesc) == 42);
}

TEST_CASE("MetaTable::TypeDescriptorOf", "[TypeDescriptor]")
{
    Type intType = MetaTable::TypeOf<int>();
    Type floatType = MetaTable::TypeOf<float>();

    SECTION("Void")
    {
        TypeDescriptor typeDesc = MetaTable::TypeDescriptorOf<void>();

        REQUIRE(typeDesc.GetUnderlyingType() == Type(TypeID<void>()));
        REQUIRE(typeDesc.GetPointerCount() == 0);
        REQUIRE(typeDesc.GetArraySize() == 0);
        REQUIRE_FALSE(typeDesc.IsConst());
        REQUIRE_FALSE(typeDesc.IsVolatile());
        REQUIRE_FALSE(typeDesc.IsReference());
        REQUIRE_FALSE(typeDesc.IsRValueReference());
    }

    SECTION("Plain type")
    {
        TypeDescriptor typeDesc = MetaTable::TypeDescriptorOf<int>();

        REQUIRE(typeDesc.GetUnderlyingType() == intType);
        REQUIRE(typeDesc.GetPointerCount() == 0);
        REQUIRE(typeDesc.GetArraySize() == 1);
        REQUIRE_FALSE(typeDesc.IsConst());
        REQUIRE_FALSE(typeDesc.IsVolatile());
        REQUIRE_FALSE(typeDesc.IsReference());
        REQUIRE_FALSE(typeDesc.IsRValueReference());
    }

    SECTION("Const reference")
    {
        TypeDescriptor typeDesc = MetaTable::TypeDescriptorOf<const int&>();

        REQUIRE(typeDesc.GetUnderlyingType() == intType);
        REQUIRE(typeDesc.GetPointerCount() == 0);
        REQUIRE(typeDesc.GetArraySize() == 1);
        REQUIRE(typeDesc.IsConst());
        REQUIRE_FALSE(typeDesc.IsVolatile());
        REQUIRE(typeDesc.IsReference());
        REQUIRE_FALSE(typeDesc.IsRValueReference());
    }

    SECTION("RValue reference")
    {
        TypeDescriptor typeDesc = MetaTable::TypeDescriptorOf<int&&>();

        REQUIRE(typeDesc.GetUnderlyingType() == intType);
        REQUIRE(typeDesc.GetPointerCount() == 0);
        REQUIRE(typeDesc.GetArraySize() == 1);
        REQUIRE_FALSE(typeDesc.IsConst());
        REQUIRE_FALSE(typeDesc.IsVolatile());
        REQUIRE(typeDesc.IsReference());
        REQUIRE(typeDesc.IsRValueReference());
    }

    SECTION("Const array of pointers")
    {
        TypeDescriptor typeDesc = MetaTable::TypeDescriptorOf<volatile const int** [4]>();

        REQUIRE(typeDesc.GetUnderlyingType() == intType);
        REQUIRE(typeDesc.GetPointerCount() == 2);
        REQUIRE(typeDesc.GetArraySize() == 4);
        REQUIRE(typeDesc.IsConst());
        REQUIRE(typeDesc.IsVolatile());
        REQUIRE_FALSE(typeDesc.IsReference());
        REQUIRE_FALSE(typeDesc.IsRValueReference());
    }
}