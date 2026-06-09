#pragma once

#include "Waterlily/Core/Logging/Logger.hpp"
#include "Waterlily/Core/String/StringID.hpp"

namespace Wl
{

    class WL_CORE_API ConsoleLoggerWriter : public LoggerWriter
    {
    public:
        inline static const StringID Name = WL_SID("Console");

        virtual void Write(const char* formattedTime,
                           LogSeverity severity,
                           const char* channel,
                           const char* file,
                           uint32_t line,
                           const char* message) override;
    };

}// namespace Wl