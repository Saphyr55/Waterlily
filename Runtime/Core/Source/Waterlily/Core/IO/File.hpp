#pragma once

#include "Waterlily/Core/Containers/Array.hpp"
#include "Waterlily/Core/IO/Stream.hpp"

namespace Wl
{

    class File : public Stream
    {
    public:
        virtual int64_t Tell() = 0;

        virtual bool Seek(int64_t position) = 0;

        virtual size_t GetSize() = 0;

        virtual bool Read(uint8_t* destination, size_t nbytes) = 0;

        virtual Array<uint8_t> ReadAllBytes() = 0;
        virtual bool ReadAllBytes(Allocator* allocator, uint8_t** outDestination, size_t* outSize) = 0;

        virtual bool Write(const uint8_t* source, size_t nbytes) = 0;

        virtual bool Flush() = 0;

        virtual bool Close() = 0;

        virtual ~File() = default;
    };

}// namespace Wl