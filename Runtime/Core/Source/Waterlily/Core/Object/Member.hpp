#pragma once

#include "Type.hpp"
#include "Waterlily/Core/Memory/Memory.hpp"
#include "Waterlily/Core/Traits/AlignedStorage.hpp"

namespace Wl
{

    class Member
    {
    public:
        inline Type GetOwnerType() const
        {
            return m_type;
        }

        inline uint32 GetOffset() const
        {
            return m_offset;
        }

    public:
        constexpr Member() = default;
        constexpr explicit Member(Type type, uint32 offset)
            : m_type(type)
            , m_offset(offset)
        {
        }
        constexpr ~Member() = default;

    private:
        Type m_type;
        uint32 m_offset = 0;
    };

    template<class ObjectType, class MemberType>
    inline usize MemberOffset(MemberType ObjectType::* member)
    {
        AlignedStorageType<ObjectType> buffer;
        const uint8* base = reinterpret_cast<const uint8*>(&(buffer.GetPtr()->*member));
        return base - buffer.Storage;
    }

    template<typename T>
    inline usize MemberOffsetAlignUp(usize offset, usize alignment, usize& outMemberOffset)
    {
        offset = Memory::AlignUp(offset, alignment);
        outMemberOffset = offset;
        offset += sizeof(T);
        return offset;
    }

}// namespace Wl
