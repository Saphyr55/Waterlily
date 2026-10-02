#pragma once

#include "TypeInfo.hpp"
#include "Waterlily/Core/Asserts.hpp"
#include "Waterlily/Core/Containers/HashMap.hpp"
#include "Waterlily/Core/Containers/Set.hpp"
#include "Waterlily/Core/CoreExports.hpp"
#include "Waterlily/Core/Logging/Trace.hpp"
#include "Waterlily/Core/Object/Member.hpp"
#include "Waterlily/Core/Object/MemberInfo.hpp"
#include "Waterlily/Core/Object/Type.hpp"
#include "Waterlily/Core/String/StringID.hpp"


namespace Wl
{

    struct TypeTable
    {
        OrderedSet<MemberInfo> memberInfoMap;
        HashMap<StringID, uint32> nameToOffsetMap;
        HashMap<StringID, MethodInfo> methodMap;
        HashMap<StringID, PropertyInfo> propertyMap;
        Function<void()> registerBindings;

        TypeTable() = default;
        ~TypeTable() = default;
    };

    class WL_CORE_API MetaTable
    {
    public:
        template<typename T>
        constexpr inline static TypeDescriptor TypeDescriptorOf();

        template<>
        inline TypeDescriptor TypeDescriptorOf<void>()
        {
            return TypeDescriptor(TypeOf<void>());
        }

    public:
        static void Init();
        static void Shutdown();

        template<typename T, typename InheritType = void>
        inline static Type TypeOf();

        inline static Type GetType(const StringID& name)
        {
            return Type(TypeID(name));
        }

        template<typename T>
        inline static bool ContainsType()
        {
            return ContainsType(TypeID<T>());
        }

        static bool ContainsType(const StringID& name)
        {
            return ContainsType(TypeID(name));
        }

        static bool ContainsType(Type type)
        {
            return ContainsType(type.GetID());
        }

        static bool ContainsType(IdentifierType id)
        {
            return GetTypes().Contains(id);
        }

        template<typename T>
        inline static const TypeInfo& GetTypeInfo()
        {
            return GetTypeInfo(TypeID<T>());
        }

        static const TypeInfo& GetTypeInfo(const StringID& name)
        {
            return GetTypeInfo(name.GetHash());
        }

        static const TypeInfo& GetTypeInfo(IdentifierType id)
        {
            return GetTypes().Get(id);
        }

    public:
        template<typename T, typename ObjectType>
        static T& ReadMember(ObjectType& object, const StringID& name);

        template<typename ObjectType, typename MemberType>
        static void RegisterMember(const StringID& name, MemberType ObjectType::* member);

        static Member GetMember(Type type, const StringID& name);
        static const MemberInfo& GetMemberInfo(Type type, const StringID& name);

        static bool ContainsMember(Type type, const StringID& name);

        template<typename ObjectType, typename ReturnType, typename... ParamTypes>
        inline static void RegisterMethod(const StringID& name, ReturnType (ObjectType::*method)(ParamTypes...))
        {
            RegisterMethodImpl<decltype(method)>(name, method);
        }

        template<typename ObjectType, typename ReturnType, typename... ParamTypes>
        inline static void RegisterMethod(const StringID& name, ReturnType (ObjectType::*method)(ParamTypes...) const)
        {
            RegisterMethodImpl<decltype(method)>(name, method);
        }

        static bool ConstainsMethod(Type type, const StringID& name);
        static const MethodInfo& GetMethodInfo(Type type, const StringID& name);

        static void RegisterProperty(Type type, const StringID& name, const StringID& getterName, const StringID& setterName = "");

        static bool ConstainsProperty(Type type, const StringID& name);
        static const PropertyInfo& GetPropertyInfo(Type type, const StringID& name);

        template<typename PropertyType, typename ObjectType>
        static PropertyType ReadProperty(ObjectType& object, const StringID& name);

        template<typename ObjectType, typename PropertyType>
        static void WriteProperty(ObjectType& object, const StringID& name, const PropertyType& value);

        static const OrderedSet<MemberInfo>& GetMembers(Type type);
        static const HashMap<StringID, PropertyInfo>& GetProperties(Type type);
        static const HashMap<StringID, MethodInfo>& GetMethods(Type type);

    private:
        template<typename MethodType>
        inline static void RegisterMethodImpl(const StringID& name, MethodType method);

        template<typename T, typename InheritType>
        inline static Type RegisterTypeImpl();

        static void RegisterMemberImpl(
                Type objectType,
                const TypeDescriptor& typeDesc,
                const StringID& name,
                uint32 offset,
                uint32 size,
                uint32 align);

        static HashMap<IdentifierType, TypeInfo>& GetTypes();
        static TypeTable& GetTypeTable(Type type);

    public:
        MetaTable() = default;
        ~MetaTable() = default;

        MetaTable(const MetaTable&) = delete;
        MetaTable(MetaTable&&) = delete;

        MetaTable& operator=(const MetaTable&) = delete;
        MetaTable& operator=(MetaTable&&) = delete;

    public:
        static MetaTable& Get();

        HashMap<IdentifierType, TypeInfo> types;
        HashMap<Type, TypeTable> typeTableMap;
    };

    template<typename T, typename ObjectType>
    T& MetaTable::ReadMember(ObjectType& object, const StringID& name)
    {
        const MemberInfo& info = GetMemberInfo(TypeOf<ObjectType>(), name);
        uint8* base = reinterpret_cast<uint8*>(&object);
        T& value = *reinterpret_cast<T*>(base + info.offset);
        return value;
    }

    template<typename T>
    constexpr inline TypeDescriptor MetaTable::TypeDescriptorOf()
    {
        using DetailsType = TypeDescriptorDetails<T>;

        TypeDescriptor typeDesc(TypeOf<typename DetailsType::PlainType>());

        if constexpr (std::is_reference_v<T>)
        {
            typeDesc.SetReference();
        }

        if constexpr (std::is_rvalue_reference_v<T>)
        {
            typeDesc.SetRValueReference();
        }

        if constexpr (std::is_const_v<typename DetailsType::RemovedPointersType>)
        {
            typeDesc.SetConst();
        }

        if constexpr (std::is_volatile_v<typename DetailsType::RemovedPointersType>)
        {
            typeDesc.SetVolatile();
        }

        typeDesc.SetArraySize(DetailsType::GetArraySize());
        typeDesc.SetPointerCount(DetailsType::GetPointerCount());

        return typeDesc;
    }

    template<typename T, typename InheritType>
    inline Type MetaTable::RegisterTypeImpl()
    {
        Type type(TypeID<T>());
        if (ContainsType(type))
        {
            return type;
        }

        MetaTable& table = Get();
        TypeInfo info = TypeInfo::Of<T, InheritType>();
        table.types.Put(info.name.GetHash(), info);
        TypeTable typeTable = {};
        typeTable.registerBindings = []()
        {
            const TypeInfo& info = MetaTable::GetTypeInfo<T>();
            const char* typeName = info.name.GetData();

            WL_LOG_DEBUG("MetaTable", "Registering type: %s", typeName);

            if constexpr (requires { T::_RegisterBindings(); })
            {
                T::_RegisterBindings();
            }
        };
        table.typeTableMap.Put(type, std::move(typeTable));

        return type;
    }

    template<typename T, typename InheritType>
    inline Type MetaTable::TypeOf()
    {
        return RegisterTypeImpl<T, InheritType>();
    }

    template<typename ObjectType, typename MemberType>
    inline void MetaTable::RegisterMember(const StringID& name, MemberType ObjectType::* member)
    {
        Type objectType = MetaTable::TypeOf<ObjectType>();
        usize offset = MemberOffset(member);
        TypeDescriptor typeDesc = MetaTable::TypeDescriptorOf<MemberType>();
        usize size = sizeof(MemberType);
        usize align = alignof(MemberType);
        RegisterMemberImpl(objectType, typeDesc, name, offset, size, align);
    }

    template<typename MethodType>
    inline void MetaTable::RegisterMethodImpl(const StringID& name, MethodType method)
    {
        using Traits = MethodTraits<MethodType>;
        using ObjectType = typename Traits::Object;
        using ReturnType = typename Traits::Return;
        using ParamsType = typename Traits::Params;

        Type ownerType = MetaTable::TypeOf<ObjectType>();
        Type signature = MetaTable::TypeOf<MethodType>();
        TypeDescriptor returnVar = MetaTable::TypeDescriptorOf<ReturnType>();

        MethodInfo info;
        info.owner = ownerType;
        info.name = name;
        info.signature = signature;
        info.handle = MethodHandle::Create(method);
        info.returnVar = returnVar;
        ParamsType::ForEach([&info]<typename ParamType>(TypeTag<ParamType>)
        {
            info.paramVars.Append(MetaTable::TypeDescriptorOf<ParamType>());
        });

        MetaTable& table = MetaTable::Get();
        TypeTable& typeTable = table.typeTableMap.Get(ownerType);

        HashMap<StringID, MethodInfo>& methods = typeTable.methodMap;
        methods.Put(name, info);
    }

    template<typename PropertyType, typename ObjectType>
    PropertyType MetaTable::ReadProperty(ObjectType& object, const StringID& name)
    {
        Type objectType = MetaTable::TypeOf<ObjectType>();
        MetaTable& table = MetaTable::Get();
        TypeTable& typeTable = table.typeTableMap.Get(objectType);

        const PropertyInfo& property = MetaTable::GetPropertyInfo(objectType, name);

        WL_CHECK(property.getterMethodName != "");

        WL_CHECK_MSG(
                typeTable.methodMap.Contains(property.getterMethodName),
                "'%s' method has been not registered.",
                property.getterMethodName.GetData());

        const MethodInfo& getter = MetaTable::GetMethodInfo(objectType, property.getterMethodName);

        return getter.handle.Invoke<PropertyType>(object);
    }

    template<typename ObjectType, typename PropertyType>
    void MetaTable::WriteProperty(ObjectType& object, const StringID& name, const PropertyType& value)
    {
        Type objectType = MetaTable::TypeOf<ObjectType>();
        MetaTable& table = MetaTable::Get();
        TypeTable& typeTable = table.typeTableMap.Get(objectType);

        const PropertyInfo& property = MetaTable::GetPropertyInfo(objectType, name);

        WL_CHECK(property.setterMethodName != "");
        WL_CHECK_MSG(
                typeTable.methodMap.Contains(property.setterMethodName),
                "'%s' method has been not registered.",
                property.setterMethodName.GetData());

        const MethodInfo& setter = MetaTable::GetMethodInfo(objectType, property.setterMethodName);

        setter.handle.Invoke<void>(object, value);
    }

}// namespace Wl