#pragma once

#include "Waterlily/Core/IO/FileHandle.hpp"

namespace Wl
{

    class WL_CORE_API PlatformFileHandle : public FileHandle
    {
    public:
        virtual int64_t Tell() override;

        virtual bool Seek(int64_t position) override;

        virtual size_t GetSize() override;

        virtual bool Read(uint8_t* destination, size_t nbytes) override;

        virtual Array<uint8_t> ReadAllBytes() override;
        virtual bool ReadAllBytes(Allocator* allocator, uint8_t** outDestination, size_t* outSize) override;

        virtual bool Write(const uint8_t* source, size_t nbytes) override;

        virtual bool Flush() override;

        virtual bool Close() override;

    public:
        explicit PlatformFileHandle(FILE* stream);
        virtual ~PlatformFileHandle() override;

    private:
        FILE* m_stream;
        int64_t m_head;
    };

}// namespace Wl