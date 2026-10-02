#pragma once

#include "Waterlily/Core/CoreExports.hpp"
#include "Waterlily/Core/Defines.hpp"

namespace Wl
{

    class WL_CORE_API MemoryTrace
    {
    public:
        static void GlobalAddDeallocatedByte(uint64 size);

        static void GlobalAddAllocateByte(uint64 size);

        static uint64 GlobalGetMemoryUsage();

    private:
        static MemoryTrace& GetDefault();

        void AddDeallocatedByte(uint64 size);
        void AddAllocatedByte(uint64 size);
        uint64 GetMemoryUsage() const;

    private:
        uint64 m_memoryUsage;
    };

}// namespace Wl