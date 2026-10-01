#include "Common/Types.hpp"
#include "Waterlily/Core/Object/Object.hpp"
#include "Waterlily/Core/Serialization/JSONArchive.hpp"

#include <catch2/catch_all.hpp>

using namespace Wl;

class Value
{
public:
    explicit Value(const Type& type)
        : m_type(type)
    {
        WL_CHECK(type != MetaTable::TypeOf<void>());

        const TypeInfo& typeInfo = m_type.GetTypeInfo();
        m_data = Memory::Allocate(typeInfo.size, typeInfo.align);
        // DefaultConstruct(m_data);
    }

    ~Value()
    {
        // Destruct(m_data);
        const TypeInfo& typeInfo = m_type.GetTypeInfo();
        Memory::Deallocate(m_data, typeInfo.size, typeInfo.align);
    }

    Value(const Value&) = delete;
    Value& operator=(const Value&) = delete;

    void*& GetData()
    {
        return m_data;
    }

private:
    Type m_type;
    void* m_data = nullptr;
};

void Serialize(OutputArchive& archive, Object& object)
{
    const auto& properties = MetaTable::GetProperties(object.GetObjectType());
    for (const auto [typeName, property]: properties)
    {
        const MethodInfo& methodInfo = MetaTable::GetMethodInfo(object.GetObjectType(), property.getterMethodName);
        Type returnType = methodInfo.returnVar.GetUnderlyingType();
        TypeInfo returnTypeInfo = returnType.GetTypeInfo();
       
        Value value(returnType);
        methodInfo.handle.InvokeRaw(&object, nullptr, value.GetData());
        
        archive.Write(property.name, value.GetData(), returnType);
    }
}

void Deserialize(InputArchive& archive, Object& object)
{
    const auto& properties = MetaTable::GetProperties(object.GetObjectType());

    for (const auto [typeName, property]: properties)
    {
        const MethodInfo& methodInfo = MetaTable::GetMethodInfo(object.GetObjectType(), property.setterMethodName);
        
        WL_CHECK(methodInfo.paramVars.GetSize() == 1);

        Type paramType = methodInfo.paramVars[0].GetUnderlyingType();
        TypeInfo paramTypeInfo = paramType.GetTypeInfo();
        
        Value value(paramType);
        if (!archive.Read(property.name, value.GetData(), paramType))
        {
            WL_LOG_WARN("Serializer", "Deserialize: failed to read property '%s'", property.name.GetText().GetData());
        }
     
        methodInfo.handle.InvokeRaw(&object, &value.GetData(), nullptr);
    }
}

TEST_CASE("Serialize and Deserialize Object", "[Serializer]")
{
    Foo foo;
    foo.SetX(21);

    JSONOutputArchive outputArchive;
    Serialize(outputArchive, foo);

    JSONInputArchive inputArchive(outputArchive.GetJson());
    Foo foo2;
    Deserialize(inputArchive, foo2);

    REQUIRE(foo2.GetX() == 21);
}
