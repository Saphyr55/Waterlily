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

        inline uint32_t GetOffset() const
        {
            return m_offset;
        }

    public:
        constexpr Member() = default;
        constexpr explicit Member(Type type, uint32_t offset)
            : m_type(type)
            , m_offset(offset)
        {
        }
        constexpr ~Member() = default;

    private:
        Type m_type;
        uint32_t m_offset = 0;
    };

    template<class ObjectType, class MemberType>
    inline size_t MemberOffset(MemberType ObjectType::* member)
    {
        AlignedStorageType<ObjectType> buffer;
        const uint8_t* base = reinterpret_cast<const uint8_t*>(&(buffer.GetPtr()->*member));
        return base - buffer.Storage;
    }

    template<typename T>
    inline size_t MemberOffsetAlignUp(size_t offset, size_t alignment, size_t& outMemberOffset)
    {
        offset = Memory::AlignUp(offset, alignment);
        outMemberOffset = offset;
        offset += sizeof(T);
        return offset;
    }

}// namespace Wl
