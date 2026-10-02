#pragma once

#include "Waterlily/Core/Containers/Array.hpp"
#include "Waterlily/Core/IO/Stream.hpp"

namespace Wl
{

    class File : public Stream
    {
    public:
        virtual int64 Tell() = 0;

        virtual bool Seek(int64 position) = 0;

        virtual usize GetSize() = 0;

        virtual bool Read(uint8* destination, usize nbytes) = 0;

        virtual Array<uint8> ReadAllBytes() = 0;
        virtual bool ReadAllBytes(Allocator* allocator, uint8** outDestination, usize* outSize) = 0;

        virtual bool Write(const uint8* source, usize nbytes) = 0;

        virtual bool Flush() = 0;

        virtual bool Close() = 0;

        virtual ~File() = default;
    };

}// namespace Wl