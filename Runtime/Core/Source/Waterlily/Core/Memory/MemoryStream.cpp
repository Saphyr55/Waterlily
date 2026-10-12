#include "Waterlily/Core/Memory/MemoryStream.hpp"
#include "Waterlily/Core/Memory/Memory.hpp"

namespace Wl
{

    bool MemoryStream::Read(uint8* destination, usize nbytes)
    {
        if (m_head + nbytes > m_size)
        {
            return false;
        }

        Memory::Copy(destination, &m_buffer[m_head], nbytes);

        m_head += nbytes;

        return true;
    }

    bool MemoryStream::Write(const uint8* source, usize nbytes)
    {
        if (m_head + nbytes > m_size)
        {
            return false;
        }

        Memory::Copy(&m_buffer[m_head], source, nbytes);

        m_head += nbytes;

        return true;
    }

    bool MemoryStream::Flush()
    {
        return true;
    }

    int64 MemoryStream::Tell()
    {
        return m_head;
    }

    bool MemoryStream::Seek(int64 position)
    {
        m_head = position;
        return true;
    }

    usize MemoryStream::GetSize()
    {
        return m_size;
    }

    MemoryStream::MemoryStream(uint8* buffer, usize size)
        : m_buffer(buffer)
        , m_size(size)
        , m_head(0)
    {
    }

    MemoryStream::~MemoryStream()
    {
    }

}// namespace Wl
