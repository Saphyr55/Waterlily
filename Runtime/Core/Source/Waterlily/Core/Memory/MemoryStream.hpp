#pragma once

#include "Waterlily/Core/CoreExports.hpp"
#include "Waterlily/Core/Defines.hpp"
#include "Waterlily/Core/IO/Stream.hpp"

namespace Wl
{

    class WL_CORE_API MemoryStream : public Stream
    {
    public:
        virtual bool Read(uint8_t* destination, size_t nbytes) override;

        virtual bool Write(const uint8_t* source, size_t nbytes) override;

        virtual bool Flush() override;

        virtual int64_t Tell() override;

        virtual bool Seek(int64_t position) override;

        virtual size_t GetSize() override;

    public:
        MemoryStream(uint8_t* buffer, size_t size);
        virtual ~MemoryStream() override;

    private:
        uint8_t* m_buffer;
        size_t m_size;
        int64_t m_head;
    };

}// namespace Wl
