#pragma once

#include "Waterlily/Core/Defines.hpp"
#include "Waterlily/Core/Hash/fnv-1a.hpp"
#include "Waterlily/Core/String/StringID.hpp"

#include <cstdint>
#include <string>
#include <string_view>

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

    enum class TypeKind : uint8
    {
        Void,
        Primitive,
        String,
        Object,
    };

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

    WL_DEFINE_TYPE_ID(float32)
    WL_DEFINE_TYPE_ID(float64)

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

        // TODO: Use StringRef but for now std::string_view offers a lot functions for handling string.
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
        if (name == "void")
        {
            return TypeID<void>();
        }
        return name.GetHash();
    }

    struct TypeInfo
    {
        StringID name;
        StringID inherit;
        uint32 size;
        uint32 align;
        TypeKind kind;

        template<typename Type, typename InheritType = void>
        inline static TypeInfo Of() noexcept
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
                info.kind = TypeKind::Primitive;
            }
            else if constexpr (IsStringType<Type>())
            {
                info.kind = TypeKind::String;
            }
            else
            {
                info.kind = TypeKind::Object;
            }

            return info;
        }

        template<>
        inline TypeInfo Of<void, void>() noexcept
        {
            std::string name(TypeName<void>());
            std::string inherit(TypeName<void>());

            TypeInfo info = {};
            info.name = StringID(TypeID<void>(), name.c_str());
            info.inherit = StringID(TypeID<void>(), inherit.c_str());
            info.size = 0;
            info.align = 0;
            info.kind = TypeKind::Void;

            return info;
        }
    };

}// namespace Wl