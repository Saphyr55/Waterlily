#pragma once

#include "Waterlily/Core/CoreExports.hpp"
#include "Waterlily/Core/Defines.hpp"
#include "Waterlily/Core/IO/Stream.hpp"

namespace Wl
{

    class WL_CORE_API MemoryStream : public Stream
    {
    public:
        virtual bool Read(uint8* destination, usize nbytes) override;

        virtual bool Write(const uint8* source, usize nbytes) override;

        virtual bool Flush() override;

        virtual int64 Tell() override;

        virtual bool Seek(int64 position) override;

        virtual usize GetSize() override;

    public:
        MemoryStream(uint8* buffer, usize size);
        virtual ~MemoryStream() override;

    private:
        uint8* m_buffer;
        usize m_size;
        int64 m_head;
    };

}// namespace Wl
