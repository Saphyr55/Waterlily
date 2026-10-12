#pragma once

#include "Waterlily/Core/IO/File.hpp"

namespace Wl
{

    class WL_CORE_API PlatformFile : public File
    {
    public:
        virtual int64 Tell() override;

        virtual bool Seek(int64 position) override;

        virtual usize GetSize() override;

        virtual bool Read(uint8* destination, usize nbytes) override;

        virtual Array<uint8> ReadAllBytes() override;
        virtual bool ReadAllBytes(Allocator* allocator, uint8** outDestination, usize* outSize) override;

        virtual bool Write(const uint8* source, usize nbytes) override;

        virtual bool Flush() override;

        virtual bool Close() override;

    public:
        explicit PlatformFile(FILE* stream);
        virtual ~PlatformFile() override;

    private:
        FILE* m_stream;
        int64 m_head;
    };

}// namespace Wl