#pragma once

#include "Waterlily/Core/Defines.hpp"
#include "Waterlily/Core/Hash/fnv-1a.hpp"
#include "Waterlily/Core/String/StringID.hpp"

#include <string>
#include <string_view>
#include <type_traits>

#define WL_DEFINE_TYPE_ID(Type)             \
    template<>                              \
    consteval IdentifierType TypeID<Type>() \
    {                                       \
        return fnv1a_cstr(#Type);           \
    }

#define WL_DEFINE_TYPE_NAME(Type)               \
    template<>                                  \
    consteval std::string_view TypeName<Type>() \
    {                                           \
        return #Type;                           \
    }


namespace Wl
{
    using IdentifierType = uint64;

    // TODO: Use StringRef but for now std::string_view offers a lot functions for handling string.
    template<typename T>
    consteval std::string_view TypeName();

    WL_DEFINE_TYPE_NAME(void)
    WL_DEFINE_TYPE_NAME(bool)
    WL_DEFINE_TYPE_NAME(char)

    WL_DEFINE_TYPE_NAME(uint8)
    WL_DEFINE_TYPE_NAME(uint16)
    WL_DEFINE_TYPE_NAME(uint32)
    WL_DEFINE_TYPE_NAME(uint64)

    WL_DEFINE_TYPE_NAME(int8)
    WL_DEFINE_TYPE_NAME(int16)
    WL_DEFINE_TYPE_NAME(int32)
    WL_DEFINE_TYPE_NAME(int64)

    WL_DEFINE_TYPE_NAME(float)
    WL_DEFINE_TYPE_NAME(double)

    template<typename T>
    consteval IdentifierType TypeID();

    WL_DEFINE_TYPE_ID(void)
    WL_DEFINE_TYPE_ID(bool)
    WL_DEFINE_TYPE_ID(char)

    WL_DEFINE_TYPE_ID(uint8)
    WL_DEFINE_TYPE_ID(uint16)
    WL_DEFINE_TYPE_ID(uint32)
    WL_DEFINE_TYPE_ID(uint64)

    WL_DEFINE_TYPE_ID(int8)
    WL_DEFINE_TYPE_ID(int16)
    WL_DEFINE_TYPE_ID(int32)
    WL_DEFINE_TYPE_ID(int64)

    WL_DEFINE_TYPE_ID(float)
    WL_DEFINE_TYPE_ID(double)

    template<typename Type>
    consteval bool IsPrimitiveType()
    {
        return std::is_same_v<Type, bool> ||
               std::is_same_v<Type, char> ||

               std::is_same_v<Type, uint8> ||
               std::is_same_v<Type, uint16> ||
               std::is_same_v<Type, uint32> ||
               std::is_same_v<Type, uint64> ||

               std::is_same_v<Type, int8> ||
               std::is_same_v<Type, int16> ||
               std::is_same_v<Type, int32> ||
               std::is_same_v<Type, int64> ||

               std::is_same_v<Type, float> ||
               std::is_same_v<Type, double>;
    }

    template<typename Type>
    consteval bool IsStringType()
    {
        return std::is_same_v<Type, String> ||
               std::is_same_v<Type, StringRef> ||
               std::is_same_v<Type, std::string> ||
               std::is_same_v<Type, std::string_view>;
    }

    namespace Internal
    {
        using ProberType = void;

        template<typename T>
        consteval std::string_view WrappedTypeName()
        {
            return WL_PRETTY_FUNCTION();
        }

        consteval usize WrappedTypeNamePrefixLength()
        {
            return WrappedTypeName<ProberType>().find(TypeName<ProberType>());
        }

        consteval usize WrappedTypeNameSuffixLength()
        {
            return WrappedTypeName<ProberType>().length() - WrappedTypeNamePrefixLength() - TypeName<ProberType>().length();
        }

    }// namespace Internal

    template<typename T>
    consteval std::string_view TypeName()
    {
        constexpr std::string_view wrappedName = Internal::WrappedTypeName<T>();
        constexpr usize prefixLength = Internal::WrappedTypeNamePrefixLength();
        constexpr usize suffixLength = Internal::WrappedTypeNameSuffixLength();
        constexpr usize nameLength = wrappedName.length() - prefixLength - suffixLength;
        return wrappedName.substr(prefixLength, nameLength);
    }

    template<typename T>
    consteval IdentifierType TypeID()
    {
        constexpr std::string_view typeName = TypeName<T>();
        return fnv1a_cstr(typeName.data(), typeName.size());
    }

    inline IdentifierType TypeID(const StringID& name)
    {
        return name.GetHash();
    }

    class TypeInfo
    {
    public:
        enum class Kind : uint8
        {
            Void,
            Primitive,
            String,
            Array,
            Map,
            Object,
        };

        using DefaultConstructDelegate = void(void*);
        using DestructDelegate = void(void*);
        using CopyDelegate = void(void*);
        using MoveDelegate = void(void*);

    public:
        StringID name;
        StringID inherit;
        uint32 size;
        uint32 align;
        Kind kind;

        DefaultConstructDelegate* DefaultConstruct = nullptr;
        DestructDelegate* Destruct = nullptr;
        CopyDelegate* Copy = nullptr;
        MoveDelegate* Move = nullptr;

    public:
        template<typename Type, typename InheritType = void>
        inline static TypeInfo Of() noexcept;
    };

    template<typename Type, typename InheritType>
    inline TypeInfo TypeInfo::Of() noexcept
    {
        std::string name(TypeName<Type>());
        std::string inherit(TypeName<InheritType>());

        TypeInfo info = {};
        info.name = StringID(TypeID<Type>(), name.c_str());
        info.inherit = StringID(TypeID<InheritType>(), inherit.c_str());
        info.size = sizeof(Type);
        info.align = alignof(Type);

        if constexpr (IsPrimitiveType<Type>())
        {
            info.kind = Kind::Primitive;
        }
        else if constexpr (IsStringType<Type>())
        {
            info.kind = Kind::String;
        }
        else
        {
            info.kind = Kind::Object;
        }

        if constexpr (std::is_default_constructible_v<Type>)
        {
            info.DefaultConstruct = [](void* data)
            {
                new (data) Type();
            };
        }

        if constexpr (std::is_destructible_v<Type>)
        {
            info.Destruct = [](void* data)
            {
                static_cast<Type*>(data)->~Type();
            };
        }

        return info;
    }

    template<>
    inline TypeInfo TypeInfo::Of<void, void>() noexcept
    {
        std::string name(TypeName<void>());
        std::string inherit(TypeName<void>());

        TypeInfo info = {};
        info.name = StringID(TypeID<void>(), name.c_str());
        info.inherit = StringID(TypeID<void>(), inherit.c_str());
        info.size = 0;
        info.align = 0;
        info.kind = Kind::Void;

        return info;
    }

}// namespace Wl