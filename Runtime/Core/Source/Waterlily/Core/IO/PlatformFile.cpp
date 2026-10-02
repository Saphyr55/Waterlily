#include "Waterlily/Core/IO/PlatformFile.hpp"
#include "Waterlily/Core/Containers/Array.hpp"
#include "Waterlily/Core/Memory/Allocator.hpp"

namespace Wl
{

    int64 PlatformFile::Tell()
    {
        return ftell(m_stream);
    }

    bool PlatformFile::Seek(int64 position)
    {
        if (fseek(m_stream, position, SEEK_SET))
        {
            m_head = position;
            return true;
        }

        return false;
    }

    bool PlatformFile::Flush()
    {
        fflush(m_stream);
        return true;
    }

    usize PlatformFile::GetSize()
    {
        int64 current = Tell();
        fseek(m_stream, 0, SEEK_END);
        int64 end = ftell(m_stream);
        Seek(current);
        return static_cast<usize>(end);
    }

    bool PlatformFile::Write(const uint8* buffer, usize nbytes)
    {
        if (!m_stream)
        {
            return false;
        }

        usize bytesWrite = fwrite(buffer, sizeof(uint8), nbytes, m_stream);
        if (bytesWrite != nbytes)
        {
            return false;
        }

        m_head += bytesWrite;

        return true;
    }

    bool PlatformFile::Read(uint8* destination, usize nbytes)
    {
        if (!destination || nbytes == 0)
        {
            return false;
        }

        usize bytesRead = fread(destination, sizeof(uint8), nbytes, m_stream);
        if (bytesRead != nbytes)
        {
            return false;
        }

        m_head += bytesRead;

        return true;
    }

    bool PlatformFile::ReadAllBytes(Allocator* allocator, uint8** outDestination, usize* outSize)
    {
        *outSize = 0;
        *outDestination = nullptr;

        if (!m_stream)
        {
            return false;
        }

        // Save the current position.
        int64 originalPos = ftell(m_stream);
        if (originalPos == -1L)
        {
            return false;
        }

        // Seek to end to determine size.
        if (fseek(m_stream, 0, SEEK_END) != 0)
        {
            return false;
        }

        int64 fileSize = ftell(m_stream);
        if (fileSize == -1L)
        {
            return false;
        }

        // Seek back to beginning.
        if (fseek(m_stream, 0, SEEK_SET) != 0)
        {
            return false;
        }

        // Allocate buffer.
        usize sizeBytes = static_cast<usize>(fileSize);

        *outSize = sizeBytes;
        *outDestination = static_cast<uint8*>(allocator->Allocate(sizeBytes, 1));

        usize bytesRead = fread(*outDestination, sizeof(uint8), *outSize, m_stream);
        if (bytesRead != fileSize)
        {
            return false;// Read error or incomplete read.
        }

        // Restore original position.
        fseek(m_stream, originalPos, SEEK_SET);

        return true;
    }

    Array<uint8> PlatformFile::ReadAllBytes()
    {
        if (!m_stream)
        {
            return {};
        }

        // Save the current position.
        int64 originalPos = ftell(m_stream);
        if (originalPos == -1L)
        {
            return {};
        }

        // Seek to end to determine size.
        if (fseek(m_stream, 0, SEEK_END) != 0)
        {
            return {};
        }

        int64 fileSize = ftell(m_stream);
        if (fileSize == -1L)
        {
            return {};
        }

        // Seek back to beginning.
        if (fseek(m_stream, 0, SEEK_SET) != 0)
        {
            return {};
        }

        // Allocate buffer.
        Array<uint8> buffer;
        buffer.Resize(static_cast<usize>(fileSize));
        usize bytesRead = fread(buffer.GetData(), sizeof(uint8), buffer.GetSize(), m_stream);
        if (bytesRead != buffer.GetSize())
        {
            return {};// Read error or incomplete read.
        }

        // Restore original position.
        fseek(m_stream, originalPos, SEEK_SET);

        return buffer;
    }

    bool PlatformFile::Close()
    {
        if (m_stream)
        {
            fclose(m_stream);
            m_stream = nullptr;
            return true;
        }

        return false;
    }

    PlatformFile::PlatformFile(FILE* stream)
        : m_stream(stream)
        , m_head(0)
    {
    }

    PlatformFile::~PlatformFile()
    {
        Close();
    }

}// namespace Wl