#pragma once

#include "Waterlily/Core/Containers/HashMap.hpp"
#include "Waterlily/Core/CoreExports.hpp"
#include "Waterlily/Core/Memory/SharedPtr.hpp"
#include "Waterlily/Core/String/StringID.hpp"

namespace Wl
{

    enum class LogSeverity : int
    {
        Trace = 0,
        Info = 1,
        Warn = 2,
        Debug = 3,
        Error = 4,
        Fatal = 5,
    };

    class LoggerWriter
    {
    public:
        virtual void Write(const char* formattedTime,
                           LogSeverity severity,
                           const char* channel,
                           const char* file,
                           uint32_t line,
                           const char* message) = 0;
    };

    class WL_CORE_API Logger
    {
    public:
        static const char* GetResetColor();

        static const char* GetSeverityColor(LogSeverity severity);
        static const char* GetSeverity(LogSeverity severity);

        static void RegisterWriter(StringID name, const SharedPtr<LoggerWriter>& writer);
        static void UnregisterWriter(StringID name);

        static void Log(LogSeverity severity,
                        const char* channel,
                        const char* file,
                        uint32_t line,
                        const char* format,
                        ...);

    private:
        static HashMap<StringID, SharedPtr<LoggerWriter>> s_writers;
    };

}// namespace Wl