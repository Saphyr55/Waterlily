#include "Waterlily/Core/Logging/ConsoleLoggerWriter.hpp"
#include "Logger.hpp"

namespace Wl
{
    void ConsoleLoggerWriter::Write(const char* formattedTime,
                                    LogSeverity severity,
                                    const char* channel,
                                    const char* file,
                                    uint32_t line,
                                    const char* message)
    {
        const char* severitycstr = Logger::GetSeverity(severity);
        const char* beginColor = Logger::GetSeverityColor(severity);
        const char* endColor = Logger::GetResetColor();

        std::fprintf(stdout, "[%s] %s[%s]%s %s\n", formattedTime, beginColor, severitycstr, endColor, message);
        if (severity > LogSeverity::Error)
        {
            std::fflush(stdout);
        }
    }

}// namespace Wl