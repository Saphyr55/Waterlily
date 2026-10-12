#include "Variant.hpp"
#include "Primitive.hpp"

namespace Wl
{

    void Variant::Reset()
    {
        Destruct();
        m_type = VoidType;
    }

    void* Variant::GetData()
    {
        if (IsNull())
        {
            return nullptr;
        }

        if (IsInline())
        {
            return m_inline;
        }

        return m_heap;
    }

    const void* Variant::GetData() const
    {
        if (IsNull())
        {
            return nullptr;
        }

        if (IsInline())
        {
            return m_inline;
        }

        return m_heap;
    }

    void Variant::Construct(Type type)
    {
        m_type = type;
        Construct();
    }

    void Variant::Construct()
    {
        if (IsNull())
        {
            return;
        }

        Reset();

        const TypeInfo& typeInfo = GetTypeInfo();

        if (!IsInline())
        {
            m_heap = Memory::Allocate(typeInfo.size, typeInfo.align);
        }

        if (typeInfo.DefaultConstruct)
        {
            typeInfo.DefaultConstruct(GetData());
        }
    }

    void Variant::Destruct()
    {
        if (IsNull())
        {
            return;
        }

        const TypeInfo& typeInfo = GetTypeInfo();

        if (typeInfo.Destruct)
        {
            typeInfo.Destruct(GetData());
        }

        if (!IsInline())
        {
            Memory::Deallocate(m_heap, typeInfo.size, typeInfo.align);
        }
    }

}// namespace Wl
