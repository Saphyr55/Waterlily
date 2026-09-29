#include "Waterlily/Core/Object/Variable.hpp"
#include "Waterlily/Core/Object/MetaTable.hpp"
#include "Waterlily/Core/Object/TypeInfo.hpp"

#include <catch2/catch_test_macros.hpp>

using namespace Wl;

template<typename FirstParamaterType, typename... ParamaterTypes>
void PrintParams()
{
    Variable var = MetaTable::VariableOf<FirstParamaterType>();
    std::cout << "\t" << var.GetVariableTypeName() << "\n";
    if constexpr (sizeof...(ParamaterTypes) != 0)
    {
        PrintParams<ParamaterTypes...>();
    }
}

template<typename ReturnType, typename... ParamaterTypes>
void PrintFunction(ReturnType (*)(ParamaterTypes...))
{
    Variable returnVar = MetaTable::VariableOf<ReturnType>();
    std::cout << "Return Type " << returnVar.GetVariableTypeName() << "\n";
    std::cout << "Params[\n";
    PrintParams<ParamaterTypes...>();
    std::cout << "]\n";
}

TEST_CASE("Variable equality", "[Variable]")
{
    Type intType = MetaTable::TypeOf<int>();
    Type floatType = MetaTable::TypeOf<float>();

    SECTION("Default variables are equal")
    {
        Variable a;
        Variable b;
        REQUIRE(a == b);
        REQUIRE_FALSE(a != b);
    }

    SECTION("Same type is equal")
    {
        Variable a(intType);
        Variable b(intType);
        REQUIRE(a == b);
        REQUIRE_FALSE(a != b);
    }

    SECTION("Different type is not equal")
    {
        Variable a(intType);
        Variable b(floatType);
        REQUIRE_FALSE(a == b);
        REQUIRE(a != b);
    }
}

TEST_CASE("Variable complex state", "[Variable]")
{
    Variable variable(MetaTable::TypeOf<int>());
    REQUIRE(variable.IsPlain());

    variable.SetConst();
    variable.AddPointer();
    variable.SetArraySize(16);

    REQUIRE_FALSE(variable.IsPlain());
    REQUIRE(variable.GetUnderlineType() == MetaTable::TypeOf<int>());
    REQUIRE(variable.IsConst());
    REQUIRE(variable.IsPointer());
    REQUIRE(variable.GetPointerCount() == 1);
    REQUIRE(variable.IsArray());
    REQUIRE(variable.GetArraySize() == 16);
}

TEST_CASE("Variable can be used as HashMap key", "[Variable]")
{
    HashMap<Variable, int> values;

    Variable intVariable(MetaTable::TypeOf<int>());
    intVariable.SetConst();

    Variable sameVariable(MetaTable::TypeOf<int>());
    sameVariable.SetConst();

    values.Emplace(intVariable, 42);

    REQUIRE(values.Contains(sameVariable));
    REQUIRE(values.Get(sameVariable) == 42);
}

TEST_CASE("MetaTable::VariableOf", "[Variable]")
{
    Type intType = MetaTable::TypeOf<int>();
    Type floatType = MetaTable::TypeOf<float>();

    SECTION("Void")
    {
        Variable var = MetaTable::VariableOf<void>();

        REQUIRE(var.GetUnderlineType() == Type(TypeID<void>()));
        REQUIRE(var.GetPointerCount() == 0);
        REQUIRE(var.GetArraySize() == 0);
        REQUIRE_FALSE(var.IsConst());
        REQUIRE_FALSE(var.IsVolatile());
        REQUIRE_FALSE(var.IsReference());
        REQUIRE_FALSE(var.IsRValueReference());
    }

    SECTION("Plain type")
    {
        Variable var = MetaTable::VariableOf<int>();

        REQUIRE(var.GetUnderlineType() == intType);
        REQUIRE(var.GetPointerCount() == 0);
        REQUIRE(var.GetArraySize() == 1);
        REQUIRE_FALSE(var.IsConst());
        REQUIRE_FALSE(var.IsVolatile());
        REQUIRE_FALSE(var.IsReference());
        REQUIRE_FALSE(var.IsRValueReference());
    }

    SECTION("Const reference")
    {
        Variable var = MetaTable::VariableOf<const int&>();

        REQUIRE(var.GetUnderlineType() == intType);
        REQUIRE(var.GetPointerCount() == 0);
        REQUIRE(var.GetArraySize() == 1);
        REQUIRE(var.IsConst());
        REQUIRE_FALSE(var.IsVolatile());
        REQUIRE(var.IsReference());
        REQUIRE_FALSE(var.IsRValueReference());
    }

    SECTION("RValue reference")
    {
        Variable var = MetaTable::VariableOf<int&&>();

        REQUIRE(var.GetUnderlineType() == intType);
        REQUIRE(var.GetPointerCount() == 0);
        REQUIRE(var.GetArraySize() == 1);
        REQUIRE_FALSE(var.IsConst());
        REQUIRE_FALSE(var.IsVolatile());
        REQUIRE(var.IsReference());
        REQUIRE(var.IsRValueReference());
    }

    SECTION("Const array of pointers")
    {
        Variable var = MetaTable::VariableOf<volatile const int** [4]>();

        REQUIRE(var.GetUnderlineType() == intType);
        REQUIRE(var.GetPointerCount() == 2);
        REQUIRE(var.GetArraySize() == 4);
        REQUIRE(var.IsConst());
        REQUIRE(var.IsVolatile());
        REQUIRE_FALSE(var.IsReference());
        REQUIRE_FALSE(var.IsRValueReference());
    }
}