#pragma once

#include "Waterlily/Core/Defines.hpp"
#include "Waterlily/Core/Hash/fnv-1a.hpp"
#include "Waterlily/Core/String/String.hpp"
#include "Waterlily/Core/String/StringID.hpp"
#include "Waterlily/Core/String/StringRef.hpp"

#include <cstdint>
#include <string>
#include <string_view>

namespace Wl
{

    template<typename T>
    consteval std::string_view TypeName();

    template<>
    consteval std::string_view TypeName<void>()
    {
        return "void";
    }

    template<typename T>
    consteval uint64_t TypeID();

    template<>
    consteval uint64_t TypeID<void>()
    {
        return 0;
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

        consteval size_t WrappedTypeNamePrefixLength()
        {
            return WrappedTypeName<ProberType>().find(TypeName<ProberType>());
        }

        consteval size_t WrappedTypeNameSuffixLength()
        {
            return WrappedTypeName<ProberType>().length() - WrappedTypeNamePrefixLength() - TypeName<ProberType>().length();
        }

    }// namespace Internal

    template<typename T>
    consteval std::string_view TypeName()
    {
        constexpr std::string_view wrappedName = Internal::WrappedTypeName<T>();
        constexpr size_t prefixLength = Internal::WrappedTypeNamePrefixLength();
        constexpr size_t suffixLength = Internal::WrappedTypeNameSuffixLength();
        constexpr size_t nameLength = wrappedName.length() - prefixLength - suffixLength;
        return wrappedName.substr(prefixLength, nameLength);
    }

    template<typename T>
    consteval uint64_t TypeID()
    {
        constexpr std::string_view typeName = TypeName<T>();
        return fnv1a_cstr(typeName.data(), typeName.size());
    }

    struct TypeInfo
    {
        StringID Name;
        uint32_t Size;
        uint32_t Align;

        template<typename T>
        inline static TypeInfo Create()
        {
            TypeInfo info = {};
            info.Name = StringID(TypeID<T>(), std::string(TypeName<T>()).c_str());
            info.Size = sizeof(T);
            info.Align = alignof(T);
            return info;
        }

        TypeInfo() = default;
        ~TypeInfo() = default;
    };

}// namespace Wl