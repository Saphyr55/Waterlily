#include "Waterlily/Core/Memory/MemoryTrace.hpp"

namespace Wl
{

    void MemoryTrace::GlobalAddDeallocatedByte(uint64_t size)
    {
        GetDefault().AddDeallocatedByte(size);
    }

    void MemoryTrace::GlobalAddAllocateByte(uint64_t size)
    {
        GetDefault().AddAllocatedByte(size);
    }

    uint64_t MemoryTrace::GlobalGetMemoryUsage()
    {
        return GetDefault().GetMemoryUsage();
    }

    MemoryTrace& MemoryTrace::GetDefault()
    {
        static MemoryTrace s_tracer;
        return s_tracer;
    }

    void MemoryTrace::AddDeallocatedByte(uint64_t size)
    {
        m_memoryUsage -= size;
    }

    void MemoryTrace::AddAllocatedByte(uint64_t size)
    {
        m_memoryUsage += size;
    }

    uint64_t MemoryTrace::GetMemoryUsage() const
    {
        return m_memoryUsage;
    }

}// namespace Wl