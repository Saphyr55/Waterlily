#include "Common/Types.hpp"
#include "Waterlily/Core/Asserts.hpp"
#include "Waterlily/Core/Memory/Memory.hpp"
#include "Waterlily/Core/Object/MemberInfo.hpp"
#include "Waterlily/Core/Object/MetaTable.hpp"
#include "Waterlily/Core/Object/Object.hpp"
#include "Waterlily/Core/Object/TypeDescriptor.hpp"
#include "Waterlily/Core/Object/TypeInfo.hpp"
#include "Waterlily/Core/Object/Variant.hpp"
#include "Waterlily/Core/Serialization/Json/JsonInputArchive.hpp"
#include "Waterlily/Core/Serialization/Json/JsonOutputArchive.hpp"
#include "Waterlily/Core/String/String.hpp"

#include <catch2/catch_test_macros.hpp>
#include <iostream>

using namespace Wl;

static void Serialize(OutputArchive& archive, Object& object)
{
    Type objectType = object.GetObjectType();
    OrderedSet<MemberInfo> memberInfos = MetaTable::GetMembers(objectType);

    uint8* base = reinterpret_cast<uint8*>(&object);

    for (const MemberInfo& member: memberInfos)
    {
        const TypeDescriptor& memberTypeDesc = member.typeDesc;
        const TypeInfo& memberTypeInfo = memberTypeDesc.GetUnderlyingType().GetTypeInfo();

        if (memberTypeInfo.kind == TypeInfo::Kind::Void)
        {
            WL_LOG_FATAL("Serialization", "Type 'void' dettected has a member type, this should be not possible. The bug comes from the MetaTable API.");
            continue;
        }

        if (memberTypeDesc.IsPointerRef())
        {
            // TODO: Pointer and Reference.
            continue;
        }

        if (MetaTable::ConstainsProperty(objectType, member.name))
        {
            const PropertyInfo& property = MetaTable::GetPropertyInfo(objectType, member.name);
            const MethodInfo& getterMethodInfo = MetaTable::GetMethodInfo(objectType, property.getterMethodName);
            MethodHandle getter = getterMethodInfo.handle;
            Type returnType = getterMethodInfo.returnVar.GetUnderlyingType();
            TypeInfo returnTypeInfo = returnType.GetTypeInfo();

            switch (memberTypeInfo.kind)
            {
                case TypeInfo::Kind::Primitive:
                {
                    Variant value(returnType);
                    getter.InvokeRaw(&object, nullptr, value.GetData());
                    archive.Write(member.name.GetText(), value.GetData(), member.typeDesc.GetUnderlyingType());
                    break;
                }
                case TypeInfo::Kind::String:
                {
                    archive.Write(member.name.GetText(), getter.Invoke<String>(&object));
                    break;
                }
                case TypeInfo::Kind::Object:
                {
                    // TODO: Object...
                    break;
                }
                default:
                    break;
            }
        }
        else
        {
            switch (memberTypeInfo.kind)
            {
                case TypeInfo::Kind::Primitive:
                case TypeInfo::Kind::String:
                {
                    archive.Write(member.name.GetText(), base + member.offset, member.typeDesc.GetUnderlyingType());
                    break;
                }
                case TypeInfo::Kind::Object:
                {
                    // TODO: Object...
                    break;
                }
                default:
                    break;
            }
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

        if (memberTypeInfo.kind == TypeInfo::Kind::Void)
        {
            WL_LOG_FATAL("Serialization", "Type 'void' dettected has a member type, this should be not possible. The bug comes from the MetaTable API.");
            continue;
        }

        if (memberTypeDesc.IsPointerRef())
        {
            // TODO: Pointer and Reference.
            continue;
        }

        if (MetaTable::ConstainsProperty(objectType, member.name))
        {
            const PropertyInfo& property = MetaTable::GetPropertyInfo(objectType, member.name);
            const MethodInfo& methodInfo = MetaTable::GetMethodInfo(objectType, property.setterMethodName);
            MethodHandle setter = methodInfo.handle;
            WL_CHECK(methodInfo.paramVars.GetSize() == 1);

            switch (memberTypeInfo.kind)
            {
                case TypeInfo::Kind::Primitive:
                {
                    Variant value(member.typeDesc.GetUnderlyingType());
                    if (!archive.Read(member.name.GetText(), value.GetData(), value.GetType()))
                    {
                        WL_LOG_ERROR("Serialization", "Deserialize: failed to read property '%s'", member.name.GetData());
                        break;
                    }
                    void* args[] = {value.GetData()};
                    setter.InvokeRaw(&object, args, nullptr);
                    break;
                }
                case TypeInfo::Kind::String:
                {
                    String value;
                    if (!archive.Read(member.name.GetText(), value))
                    {
                        WL_LOG_ERROR("Serialization", "Deserialize: failed to read property '%s'", member.name.GetData());
                        break;
                    }
                    void* args[] = {&value};
                    setter.InvokeRaw(&object, args, nullptr);
                }
                case TypeInfo::Kind::Object:
                default:
                    break;
            }
        }
        else
        {
            switch (memberTypeInfo.kind)
            {
                case TypeInfo::Kind::Primitive:
                {
                    Variant value(member.typeDesc.GetUnderlyingType());
                    if (!archive.Read(member.name.GetText(), value.GetData(), value.GetType()))
                    {
                        WL_LOG_WARN("Serialization", "Deserialize: failed to read property '%s'", member.name.GetData());
                    }
                    void* args[] = {value.GetData()};
                    Memory::Copy(base + member.offset, args, member.size);
                    break;
                }
                case TypeInfo::Kind::String:
                {
                    String value;
                    if (!archive.Read(member.name.GetText(), value))
                    {
                        WL_LOG_ERROR("Serialization", "Deserialize: failed to read property '%s'", member.name.GetData());
                        break;
                    }
                    *reinterpret_cast<String*>(base + member.offset) = std::move(value);
                    break;
                }
                case TypeInfo::Kind::Object:
                default:
                    break;
            }
        }
    }
}

TEST_CASE("Serialize and Deserialize Object", "[Serializer]")
{
    Foo foo;
    foo.SetX(21);
    foo.z = "Hello World";

    JsonOutputArchive outputArchive;
    Serialize(outputArchive, foo);

    std::cout << outputArchive.Dump(4) << '\n';

    JsonInputArchive inputArchive(outputArchive.GetJson());
    Foo foo2;
    Deserialize(inputArchive, foo2);

    std::cout << inputArchive.Dump(4) << '\n';

    REQUIRE(foo2.z == "Hello World");
    REQUIRE(foo2.GetX() == 21);
}
