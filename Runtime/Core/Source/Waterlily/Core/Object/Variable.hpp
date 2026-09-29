#pragma once

#include "Waterlily/Core/Defines.hpp"
#include "Waterlily/Core/Hash/Hasher.hpp"
#include "Waterlily/Core/Traits/Traits.hpp"
#include "Waterlily/Core/Object/Type.hpp"
#include "Waterlily/Core/Object/TypeInfo.hpp"

#include <cstdint>
#include <type_traits>

namespace Wl
{

    using ModifierBase = uint8_t;

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
    struct VariableDetails
    {
        using RemovedExtentsType = std::remove_all_extents_t<T>;
        using RemovedReferencesType = std::remove_reference_t<RemovedExtentsType>;
        using RemovedPointersType = Wl::RemoveAllPointersType<RemovedReferencesType>;
        using PlainType = std::remove_cvref_t<RemovedPointersType>;

        template<typename R = RemovedReferencesType>
        consteval inline static uint32_t GetPointerCount(uint32_t count = 0)
        {
            if constexpr (std::is_pointer_v<R>)
            {
                return GetPointerCount<std::remove_pointer_t<R>>(count + 1);
            }
            return count;
        }

        consteval inline static uint32_t GetArraySize()
            requires(!std::is_same_v<void, T>)
        {
            return sizeof(T) / sizeof(RemovedExtentsType);
        }
    };

    class Variable
    {
    public:
        template<typename T>
        constexpr inline static Variable Create()
        {
            using DetailsType = VariableDetails<T>;

            Variable var(TypeOf<typename DetailsType::PlainType>());

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

        template<>
        constexpr inline Variable Create<void>()
        {
            return Variable();
        }

    public:
        constexpr inline Type GetType() const
        {
            return m_type;
        };

        constexpr inline void SetType(Type type)
        {
            m_type = type;
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

        constexpr inline uint32_t GetPointerCount() const
        {
            return m_pointerCount;
        }

        constexpr inline bool IsPointer() const
        {
            return m_pointerCount != 0;
        }

        constexpr inline void SetPointerCount(uint32_t count)
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
            return m_arraySize != 0;
        }

        constexpr inline uint32_t GetArraySize() const
        {
            return m_arraySize;
        }

        constexpr inline void SetArraySize(uint32_t size)
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

        constexpr bool operator==(const Variable& other) const
        {
            return m_type == other.m_type &&
                   m_arraySize == other.m_arraySize &&
                   m_pointerCount == other.m_pointerCount &&
                   m_modifierFlags == other.m_modifierFlags;
        }

        constexpr bool operator!=(const Variable& other) const
        {
            return !(*this == other);
        }

    public:
        constexpr explicit Variable(Type type = Type(TypeID<void>()))
            : m_type(type)
        {
        }
        constexpr ~Variable() = default;

    private:
        Type m_type;
        uint32_t m_arraySize = 0;
        uint32_t m_pointerCount = 0;
        Modifier m_modifierFlags = Modifier::None;
    };

}// namespace Wl

WL_HASH_DEFINE(Wl::Variable, var, {
    Wl::Type type = var.GetType();
    uint64_t hash = Wl::Hash(type);
    hash = Wl::HashCombine(hash, Wl::Hash(var.GetArraySize()));
    hash = Wl::HashCombine(hash, Wl::Hash(var.GetPointerCount()));
    hash = Wl::HashCombine(hash, Wl::Hash(static_cast<Wl::ModifierBase>(var.GetModifiers())));
    return hash;
})