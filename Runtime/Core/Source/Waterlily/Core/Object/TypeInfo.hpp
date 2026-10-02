#pragma once

#include "Waterlily/Core/Defines.hpp"
#include "Waterlily/Core/Function/Function.hpp"
#include "Waterlily/Core/Hash/fnv-1a.hpp"
#include "Waterlily/Core/String/StringID.hpp"

#include <cstdint>
#include <string>
#include <string_view>

namespace Wl
{
    using IdentifierType = uint64_t;

    template<typename T>
    consteval std::string_view TypeName();

    template<>
    consteval std::string_view TypeName<void>()
    {
        return "void";
    }

    template<typename T>
    consteval IdentifierType TypeID();

    template<>
    consteval IdentifierType TypeID<void>()
    {
        return fnv1a_cstr("void");
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
        uint32_t size;
        uint32_t align;

        template<typename T, typename InheritType = void>
        inline static TypeInfo Of() noexcept
        {
            std::string name(TypeName<T>());
            std::string inherit(TypeName<InheritType>());

            TypeInfo info = {};
            info.name = StringID(TypeID<T>(), name.c_str());
            info.inherit = StringID(TypeID<InheritType>(), inherit.c_str());
            info.size = sizeof(T);
            info.align = alignof(T);

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
            return info;
        }
    };

}// namespace Wl