#pragma once

#include "Waterlily/Core/CoreExports.hpp"
#include "Waterlily/Core/Defines.hpp"
#include "Waterlily/Core/Hash/Hasher.hpp"
#include "Waterlily/Core/Object/Type.hpp"
#include "Waterlily/Core/Traits/Traits.hpp"

#include <cstdint>
#include <type_traits>

namespace Wl
{

    using ModifierBase = uint8;

    enum class Modifier : ModifierBase
    {
        None = 0,
        Const = 1 << 0,
        Reference = 1 << 1,
        Volatile = 1 << 2,
        RValueReference = 1 << 3,
    };
    WL_ENUM_FLAGS_CUSTOM_DERIVED(Modifier, ModifierBase);

    template<typename T>
    struct TypeDescriptorDetails
    {
        using RemovedExtentsType = std::remove_all_extents_t<T>;
        using RemovedReferencesType = std::remove_reference_t<RemovedExtentsType>;
        using RemovedPointersType = Wl::RemoveAllPointersType<RemovedReferencesType>;
        using PlainType = std::remove_cvref_t<RemovedPointersType>;

        template<typename R = RemovedReferencesType>
        consteval inline static uint32 GetPointerCount(uint32 count = 0)
        {
            if constexpr (std::is_pointer_v<R>)
            {
                return GetPointerCount<std::remove_pointer_t<R>>(count + 1);
            }
            return count;
        }

        consteval inline static uint32 GetArraySize()
            requires(!std::is_same_v<void, T>)
        {
            return sizeof(T) / sizeof(RemovedExtentsType);
        }
    };

    class WL_CORE_API TypeDescriptor
    {
    public:
        String GetTypeName() const;

        inline const StringID& GetUnderlyingTypeName() const
        {
            return m_underlyingType.GetTypeInfo().name;
        }

        constexpr inline Type GetUnderlyingType() const
        {
            return m_underlyingType;
        };

        constexpr inline void SetUnderlineType(Type type)
        {
            m_underlyingType = type;
        }

        constexpr inline void SetModifier(Modifier modifier)
        {
            m_modifierFlags |= modifier;
        }

        constexpr inline void RemoveModifier(Modifier modifier)
        {
            m_modifierFlags &= (~modifier);
        }

        constexpr inline Modifier GetModifiers() const
        {
            return m_modifierFlags;
        }

        constexpr inline bool IsConst() const
        {
            return HasModifier(Modifier::Const);
        }

        constexpr inline bool IsVolatile() const
        {
            return HasModifier(Modifier::Volatile);
        }

        constexpr inline void SetConst()
        {
            SetModifier(Modifier::Const);
        }

        constexpr inline void RemoveConst()
        {
            RemoveModifier(Modifier::Const);
        }

        constexpr inline void SetVolatile()
        {
            SetModifier(Modifier::Volatile);
        }

        constexpr inline void RemoveVolative()
        {
            RemoveModifier(Modifier::Volatile);
        }

        constexpr inline bool IsReference() const
        {
            return HasModifier(Modifier::Reference);
        }

        constexpr inline bool IsRValueReference() const
        {
            return HasModifier(Modifier::RValueReference);
        }

        constexpr inline bool IsLValueReference() const
        {
            return IsReference() && !IsRValueReference();
        }

        constexpr inline void SetReference()
        {
            SetModifier(Modifier::Reference);
        }

        constexpr inline void RemoveReference()
        {
            RemoveModifier(Modifier::Reference);
        }

        constexpr inline void RemoveRValueReference()
        {
            RemoveModifier(Modifier::RValueReference);
        }

        constexpr inline void SetRValueReference()
        {
            SetModifier(Modifier::RValueReference);
        }

        constexpr inline uint32 GetPointerCount() const
        {
            return m_pointerCount;
        }

        constexpr inline bool IsPointer() const
        {
            return m_pointerCount != 0;
        }

        constexpr inline void SetPointerCount(uint32 count)
        {
            m_pointerCount = count;
        }

        constexpr inline void AddPointer()
        {
            m_pointerCount++;
        }

        constexpr inline void RemovePointer()
        {
            if (m_pointerCount > 0)
            {
                m_pointerCount--;
            }
        }

        constexpr inline bool IsArray() const
        {
            return m_arraySize > 1;
        }

        constexpr inline uint32 GetArraySize() const
        {
            return m_arraySize;
        }

        constexpr inline void SetArraySize(uint32 size)
        {
            m_arraySize = size;
        }

        constexpr inline void ClearArray()
        {
            m_arraySize = 0;
        }

        constexpr inline bool IsPlain() const
        {
            return m_modifierFlags == Modifier::None && m_pointerCount == 0 && m_arraySize == 0;
        }

        constexpr inline bool HasModifiers() const
        {
            return m_modifierFlags != Modifier::None;
        }

        constexpr inline bool HasModifier(Modifier modifier) const
        {
            return (m_modifierFlags & modifier) != Modifier::None;
        }

        constexpr inline usize GetSize() const
        {
            return IsPointerRef() ? sizeof(void*) : GetArraySize() * GetUnderlyingType().GetSize();
        }

        constexpr inline usize GetAlign() const
        {
            return IsPointerRef() ? alignof(void*) : GetUnderlyingType().GetAlign();
        }

        constexpr bool IsPointerRef() const
        {
            return IsPointer() || IsReference();
        }

    public:
        constexpr bool operator==(const TypeDescriptor& other) const
        {
            return m_underlyingType == other.m_underlyingType &&
                   m_arraySize == other.m_arraySize &&
                   m_pointerCount == other.m_pointerCount &&
                   m_modifierFlags == other.m_modifierFlags;
        }

        constexpr bool operator!=(const TypeDescriptor& other) const
        {
            return !(*this == other);
        }

    public:
        constexpr TypeDescriptor() = default;
        constexpr explicit TypeDescriptor(Type type)
            : m_underlyingType(type)
        {
        }
        constexpr ~TypeDescriptor() = default;

    private:
        Type m_underlyingType;
        uint32 m_arraySize = 0;
        uint32 m_pointerCount = 0;
        Modifier m_modifierFlags = Modifier::None;
    };

}// namespace Wl

WL_HASH_DEFINE(Wl::TypeDescriptor, desc, {
    Wl::Type type = desc.GetUnderlyingType();
    uint64 hash = Wl::Hash(type);
    hash = Wl::HashCombine(hash, Wl::Hash(desc.GetArraySize()));
    hash = Wl::HashCombine(hash, Wl::Hash(desc.GetPointerCount()));
    hash = Wl::HashCombine(hash, Wl::Hash(static_cast<Wl::ModifierBase>(desc.GetModifiers())));
    return hash;
})