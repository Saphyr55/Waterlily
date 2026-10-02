#include "Waterlily/Core/Memory/MemoryTrace.hpp"

namespace Wl
{

    void MemoryTrace::GlobalAddDeallocatedByte(uint64 size)
    {
        GetDefault().AddDeallocatedByte(size);
    }

    void MemoryTrace::GlobalAddAllocateByte(uint64 size)
    {
        GetDefault().AddAllocatedByte(size);
    }

    uint64 MemoryTrace::GlobalGetMemoryUsage()
    {
        return GetDefault().GetMemoryUsage();
    }

    MemoryTrace& MemoryTrace::GetDefault()
    {
        static MemoryTrace s_tracer;
        return s_tracer;
    }

    void MemoryTrace::AddDeallocatedByte(uint64 size)
    {
        m_memoryUsage -= size;
    }

    void MemoryTrace::AddAllocatedByte(uint64 size)
    {
        m_memoryUsage += size;
    }

    uint64 MemoryTrace::GetMemoryUsage() const
    {
        return m_memoryUsage;
    }

}// namespace Wl