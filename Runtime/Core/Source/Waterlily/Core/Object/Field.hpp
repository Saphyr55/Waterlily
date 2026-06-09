#pragma once

#include "Waterlily/Core/Memory/Memory.hpp"

namespace Wl
{

    template<typename T>
    size_t FieldOffsetAlignUp(size_t offset, size_t alignment, size_t& fieldOffset)
    {
        offset = Memory::AlignUp(offset, alignment);
        fieldOffset = offset;
        offset += sizeof(T);
        return offset;
    }

}// namespace Wl

