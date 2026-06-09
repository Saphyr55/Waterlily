#include "Waterlily/Core/IO/PlatformFile.hpp"
#include "Waterlily/Core/Containers/Array.hpp"
#include "Waterlily/Core/Defines.hpp"
#include "Waterlily/Core/Memory/Allocator.hpp"

namespace Wl
{

    int64_t PlatformFile::Tell()
    {
        return ftell(m_stream);
    }

    bool PlatformFile::Seek(int64_t position)
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

    size_t PlatformFile::GetSize()
    {
        int64_t current = Tell();
        fseek(m_stream, 0, SEEK_END);
        int64_t end = ftell(m_stream);
        Seek(current);
        return static_cast<size_t>(end);
    }

    bool PlatformFile::Write(const uint8_t* buffer, size_t nbytes)
    {
        if (!m_stream)
        {
            return false;
        }

        size_t bytesWrite = fwrite(buffer, sizeof(uint8_t), nbytes, m_stream);
        if (bytesWrite != nbytes)
        {
            return false;
        }

        m_head += bytesWrite;

        return true;
    }

    bool PlatformFile::Read(uint8_t* destination, size_t nbytes)
    {
        if (!destination || nbytes == 0)
        {
            return false;
        }

        size_t bytesRead = fread(destination, sizeof(uint8_t), nbytes, m_stream);
        if (bytesRead != nbytes)
        {
            return false;
        }

        m_head += bytesRead;

        return true;
    }

    bool PlatformFile::ReadAllBytes(Allocator* allocator, uint8_t** outDestination, size_t* outSize)
    {
        *outSize = 0;
        *outDestination = nullptr;

        if (!m_stream)
        {
            return false;
        }

        // Save the current position.
        int64_t originalPos = ftell(m_stream);
        if (originalPos == -1L)
        {
            return false;
        }

        // Seek to end to determine size.
        if (fseek(m_stream, 0, SEEK_END) != 0)
        {
            return false;
        }

        int64_t fileSize = ftell(m_stream);
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
        size_t sizeBytes = static_cast<size_t>(fileSize);

        *outSize = sizeBytes;
        *outDestination = static_cast<uint8_t*>(allocator->Allocate(sizeBytes));

        size_t bytesRead = fread(*outDestination, sizeof(uint8_t), *outSize, m_stream);
        if (bytesRead != fileSize)
        {
            return false;// Read error or incomplete read.
        }

        // Restore original position.
        fseek(m_stream, originalPos, SEEK_SET);

        return true;
    }

    Array<uint8_t> PlatformFile::ReadAllBytes()
    {
        if (!m_stream)
        {
            return {};
        }

        // Save the current position.
        int64_t originalPos = ftell(m_stream);
        if (originalPos == -1L)
        {
            return {};
        }

        // Seek to end to determine size.
        if (fseek(m_stream, 0, SEEK_END) != 0)
        {
            return {};
        }

        int64_t fileSize = ftell(m_stream);
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
        Array<uint8_t> buffer;
        buffer.Resize(static_cast<size_t>(fileSize));
        size_t bytesRead = fread(buffer.GetData(), sizeof(uint8_t), buffer.GetSize(), m_stream);
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