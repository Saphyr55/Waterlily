#pragma once

#include "Waterlily/Core/Containers/HashMap.hpp"
#include "Waterlily/Core/Containers/Set.hpp"
#include "Waterlily/Core/CoreExports.hpp"
#include "Waterlily/Core/Object/Member.hpp"
#include "Waterlily/Core/Object/MemberInfo.hpp"
#include "Waterlily/Core/Object/Type.hpp"
#include "Waterlily/Core/String/StringID.hpp"


namespace Wl
{

    class TypeTable
    {
    public:
        OrderedSet<MemberInfo> memberInfoMap;
        HashMap<StringID, uint32_t> nameToOffsetMap;
        HashMap<StringID, MethodInfo> methodMap;
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
        static T ReadMember(const ObjectType& object, const StringID& name);

        static Member RegisterMember(Type objectType, const TypeDescriptor& variable, const StringID& name, uint32_t offset, uint32_t size, uint32_t align);

        template<typename ObjectType, typename MemberType>
        static Member RegisterMember(const StringID& name, MemberType ObjectType::* member);

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

    private:
        template<typename MethodType>
        inline static void RegisterMethodImpl(const StringID& name, MethodType method);

        template<typename T, typename InheritType>
        inline static void RegisterTypeImpl();

    public:
        static MetaTable& Get();

        HashMap<IdentifierType, TypeInfo> types;
        HashMap<Type, TypeTable> typeTableMap;
    };

    template<typename T, typename ObjectType>
    T MetaTable::ReadMember(const ObjectType& object, const StringID& name)
    {
        const MemberInfo& info = GetMemberInfo(TypeOf<ObjectType>(), name);
        const uint8_t* base = reinterpret_cast<const uint8_t*>(&object);
        const T value = *reinterpret_cast<const T*>(base + info.offset);
        return value;
    }

    template<typename T>
    constexpr inline TypeDescriptor MetaTable::TypeDescriptorOf()
    {
        using DetailsType = TypeDescriptorDetails<T>;

        TypeDescriptor var(TypeOf<typename DetailsType::PlainType>());

        if constexpr (std::is_reference_v<T>)
        {
            var.SetReference();
        }

        if constexpr (std::is_rvalue_reference_v<T>)
        {
            var.SetRValueReference();
        }

        if constexpr (std::is_const_v<typename DetailsType::RemovedPointersType>)
        {
            var.SetConst();
        }

        if constexpr (std::is_volatile_v<typename DetailsType::RemovedPointersType>)
        {
            var.SetVolatile();
        }

        var.SetArraySize(DetailsType::GetArraySize());
        var.SetPointerCount(DetailsType::GetPointerCount());

        return var;
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
    inline Member MetaTable::RegisterMember(const StringID& name, MemberType ObjectType::* property)
    {
        Type objectType = MetaTable::TypeOf<ObjectType>();
        size_t offset = MemberOffset(property);
        TypeDescriptor variable = MetaTable::TypeDescriptorOf<MemberType>();
        size_t size = sizeof(MemberType);
        size_t align = alignof(MemberType);
        return RegisterMember(objectType, variable, name, offset, size, align);
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

}// namespace Wl