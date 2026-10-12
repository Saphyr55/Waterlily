#pragma once

#include "MetaTable.hpp"
#include "Waterlily/Core/Defines.hpp"
#include "Waterlily/Core/Memory/Memory.hpp"
#include "Waterlily/Core/Object/Primitive.hpp"
#include "Waterlily/Core/Object/Type.hpp"
#include "Waterlily/Core/Object/TypeInfo.hpp"

namespace Wl
{

    class WL_CORE_API Variant
    {
    public:
        inline static constexpr usize InlineSize = 16;
        inline static constexpr usize InlineAlignment = alignof(std::max_align_t);

    public:
        void Reset();

        void* GetData();
        const void* GetData() const;

        inline Type GetType() const
        {
            return m_type;
        }

        inline const TypeInfo& GetTypeInfo() const
        {
            return GetType().GetTypeInfo();
        }

        inline bool IsType(Type type) const
        {
            return type == GetType();
        }

        inline bool IsNull() const
        {
            return IsType(VoidType);
        }

        inline bool IsInline() const
        {
            const TypeInfo& info = GetTypeInfo();
            return info.size <= InlineSize && info.align <= InlineAlignment;
        }

        template<typename Type, typename... Args>
        inline void Emplace(Args&&... args)
        {
            Construct(MetaTable::TypeOf<Type>());
            WL_PLACEMENT_NEW(GetData(), Type(std::forward<Args>(args)...));
        }

        template<typename Type>
        inline void Assign(const Type& type)
        {
            Construct(MetaTable::TypeOf<Type>());
            WL_PLACEMENT_NEW(GetData(), Type(type));
        }

        template<typename Type>
        inline Type* Get()
        {
            return static_cast<Type*>(GetData());
        }

        template<typename Type>
        inline const Type* Get() const
        {
            return static_cast<const Type*>(GetData());
        }

    public:
        explicit Variant(const Type& type)
            : m_type(type)
        {
            Construct(type);
        }

        ~Variant()
        {
            Destruct();
        }

    private:
        void Construct(Type type);
        void Construct();
        void Destruct();

    private:
        Type m_type;
        union
        {
            void* m_heap = nullptr;
            alignas(InlineAlignment) uint8 m_inline[InlineSize];
        };
    };

}// namespace Wl