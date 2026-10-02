#include "Common/Types.hpp"
#include "Waterlily/Core/Memory/Memory.hpp"
#include "Waterlily/Core/Object/MemberInfo.hpp"
#include "Waterlily/Core/Object/MetaTable.hpp"
#include "Waterlily/Core/Object/Object.hpp"
#include "Waterlily/Core/Object/TypeDescriptor.hpp"
#include "Waterlily/Core/Object/TypeInfo.hpp"
#include "Waterlily/Core/Serialization/Json/JsonInputArchive.hpp"
#include "Waterlily/Core/Serialization/Json/JsonOutputArchive.hpp"


#include <catch2/catch_test_macros.hpp>

using namespace Wl;

class Value
{
public:
    Type GetType() const
    {
        return m_type;
    }

    void*& GetData()
    {
        return m_data;
    }

    const void* GetData() const
    {
        return m_data;
    }

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


private:
    Type m_type;
    void* m_data = nullptr;
};

static void Serialize(OutputArchive& archive, Object& object)
{
    Type objectType = object.GetObjectType();
    OrderedSet<MemberInfo> memberInfos = MetaTable::GetMembers(objectType);

    uint8* base = reinterpret_cast<uint8*>(&object);

    for (const MemberInfo& member: memberInfos)
    {
        const TypeDescriptor& memberTypeDesc = member.typeDesc;
        const TypeInfo& memberTypeInfo = memberTypeDesc.GetUnderlyingType().GetTypeInfo();
        
        if (memberTypeDesc.IsPointerRef())
        {
            // TODO: Pointer and Reference.
            continue;
        }

        if (memberTypeInfo.kind == TypeKind::Object)
        {
            // TODO: Object.
            continue;
        }

        if (memberTypeInfo.kind == TypeKind::String)
        {
            // TODO: Object.
            continue;
        }

        // Primitive
        if (MetaTable::ConstainsProperty(objectType, member.name))
        {
            const PropertyInfo& property = MetaTable::GetPropertyInfo(objectType, member.name);
            const MethodInfo& methodInfo = MetaTable::GetMethodInfo(objectType, property.getterMethodName);
            Type returnType = methodInfo.returnVar.GetUnderlyingType();
            TypeInfo returnTypeInfo = returnType.GetTypeInfo();

            Value value(returnType);
            methodInfo.handle.InvokeRaw(&object, nullptr, value.GetData());
            archive.Write(member.name.GetText(), value.GetData(), member.typeDesc.GetUnderlyingType());
        }
        else
        {
            const void* value = reinterpret_cast<const void*>(base + member.offset);
            archive.Write(member.name.GetText(), value, member.typeDesc.GetUnderlyingType());
        }
    }
}

static void Deserialize(InputArchive& archive, Object& object)
{
    Type objectType = object.GetObjectType();
    OrderedSet<MemberInfo> memberInfos = MetaTable::GetMembers(objectType);

    uint8* base = reinterpret_cast<uint8*>(&object);

    for (const MemberInfo& member: memberInfos)
    {
        const TypeDescriptor& memberTypeDesc = member.typeDesc;
        const TypeInfo& memberTypeInfo = memberTypeDesc.GetUnderlyingType().GetTypeInfo();

        if (memberTypeDesc.IsPointerRef())
        {
            // TODO: Pointer and Reference.
            continue;
        }

        if (memberTypeInfo.kind == TypeKind::Object)
        {
            // TODO: Object.
            continue;
        }

        if (memberTypeInfo.kind == TypeKind::String)
        {
            // TODO: Object.
            continue;
        }

        Value value(member.typeDesc.GetUnderlyingType());
        if (!archive.Read(member.name.GetText(), value.GetData(), value.GetType()))
        {
            WL_LOG_WARN("Serializer", "Deserialize: failed to read property '%s'", member.name.GetData());
        }

        if (MetaTable::ConstainsProperty(objectType, member.name))
        {
            const PropertyInfo& property = MetaTable::GetPropertyInfo(objectType, member.name);
            const MethodInfo& methodInfo = MetaTable::GetMethodInfo(objectType, property.setterMethodName);
            WL_CHECK(methodInfo.paramVars.GetSize() == 1);
            methodInfo.handle.InvokeRaw(&object, &value.GetData(), nullptr);
        }
        else
        {
            Memory::Copy(base + member.offset, &value.GetData(), member.size);
        }
    }
}

TEST_CASE("Serialize and Deserialize Object", "[Serializer]")
{
    Foo foo;
    foo.SetX(21);

    JsonOutputArchive outputArchive;
    Serialize(outputArchive, foo);

    JsonInputArchive inputArchive(outputArchive.GetJson());
    Foo foo2;
    Deserialize(inputArchive, foo2);

    REQUIRE(foo2.GetX() == 21);
}
