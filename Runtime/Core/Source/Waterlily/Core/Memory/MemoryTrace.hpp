#pragma once

#include "Waterlily/Core/CoreExports.hpp"
#include "Waterlily/Core/Defines.hpp"

namespace Wl
{

    class WL_CORE_API MemoryTrace
    {
    public:
        static void GlobalAddDeallocatedByte(uint64_t size);

        static void GlobalAddAllocateByte(uint64_t size);

        static uint64_t GlobalGetMemoryUsage();

    private:
        static MemoryTrace& GetDefault();

        void AddDeallocatedByte(uint64_t size);
        void AddAllocatedByte(uint64_t size);
        uint64_t GetMemoryUsage() const;

    private:
        uint64_t m_memoryUsage;
    };

}// namespace Wl