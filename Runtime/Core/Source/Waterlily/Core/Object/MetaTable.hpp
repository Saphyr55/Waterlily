#pragma once

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
        HashMap<StringID, uint32_t> nameToOffsetMap;
        HashMap<StringID, MethodInfo> methodMap;
        HashMap<StringID, PropertyInfo> propertyMap;
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
        template<typename T, typename InheritType = void>
        inline static Type TypeOf();
    
        inline static Type GetType(const StringID& name)
        {
            WL_CHECK(ContainsType(name));
            return Type(name.GetHash());
        }

        template<typename T>
        inline static bool ContainsType()
        {
            return ContainsType(TypeID<T>());
        }

        inline static bool ContainsType(const StringID& name)
        {
            return ContainsType(name.GetHash());
        }

        inline static bool ContainsType(IdentifierType id)
        {
            return Get().types.Contains(id);
        }

        template<typename T>
        inline static const TypeInfo& GetTypeInfo()
        {
            return GetTypeInfo(TypeID<T>());
        }

        inline static const TypeInfo& GetTypeInfo(const StringID& name)
        {
            return GetTypeInfo(name.GetHash());
        }

        inline static const TypeInfo& GetTypeInfo(IdentifierType id)
        {
            return Get().types.Get(id);
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

        inline static const OrderedSet<MemberInfo>& GetMembers(Type type)
        {
            return Get().typeTableMap.Get(type).memberInfoMap;
        }

        inline static const HashMap<StringID, PropertyInfo>& GetProperties(Type type)
        {
            return Get().typeTableMap.Get(type).propertyMap;
        }

        inline static const HashMap<StringID, MethodInfo>& GetMethods(Type type)
        {
            return Get().typeTableMap.Get(type).methodMap;
        }

    private:
        template<typename MethodType>
        inline static void RegisterMethodImpl(const StringID& name, MethodType method);

        template<typename T, typename InheritType>
        inline static void RegisterTypeImpl();

        static void RegisterMemberImpl(
                Type objectType,
                const TypeDescriptor& typeDesc,
                const StringID& name,
                uint32_t offset,
                uint32_t size,
                uint32_t align);

    public:
        static MetaTable& Get();

        HashMap<IdentifierType, TypeInfo> types;
        HashMap<Type, TypeTable> typeTableMap;
    };
    
    template<typename T, typename ObjectType>
    T& MetaTable::ReadMember(ObjectType& object, const StringID& name)
    {
        const MemberInfo& info = GetMemberInfo(TypeOf<ObjectType>(), name);
        uint8_t* base = reinterpret_cast<uint8_t*>(&object);
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
    inline void MetaTable::RegisterTypeImpl()
    {
        MetaTable& table = Get();
        TypeInfo info = TypeInfo::Of<T, InheritType>();
        table.types.Put(info.name.GetHash(), info);
        table.typeTableMap.Put(Type(TypeID<T>()), TypeTable());

        if constexpr (requires { T::_RegisterBindings(); })
        {
            T::_RegisterBindings();
        }
    }

    template<typename T, typename InheritType>
    inline Type MetaTable::TypeOf()
    {
        if (!ContainsType<T>())
        {
            RegisterTypeImpl<T, InheritType>();
        }
        return Type(TypeID<T>());
    }

    template<typename ObjectType, typename MemberType>
    inline void MetaTable::RegisterMember(const StringID& name, MemberType ObjectType::* member)
    {
        Type objectType = MetaTable::TypeOf<ObjectType>();
        size_t offset = MemberOffset(member);
        TypeDescriptor typeDesc = MetaTable::TypeDescriptorOf<MemberType>();
        size_t size = sizeof(MemberType);
        size_t align = alignof(MemberType);
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
                property.getterMethodName.GetText().GetData());

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
                property.setterMethodName.GetText().GetData());

        const MethodInfo& setter = MetaTable::GetMethodInfo(objectType, property.setterMethodName);

        setter.handle.Invoke<void>(object, value);
    }

}// namespace Wl