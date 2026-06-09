#include "Waterlily/Core/Logging/Logger.hpp"
#include "Waterlily/Core/Containers/HashMap.hpp"
#include "Waterlily/Core/Memory/SharedPtr.hpp"
#include "Waterlily/Core/String/Format.hpp"
#include "Waterlily/Core/String/StringRef.hpp"

#include <chrono>
#include <cstdarg>

namespace Wl
{
    HashMap<StringID, SharedPtr<LoggerWriter>> Logger::s_writers;

    static constexpr const char* s_colorReset = "\033[0m";

    const char* Logger::GetResetColor()
    {
        return s_colorReset;
    }

    const char* Logger::GetSeverityColor(LogSeverity severity)
    {
        switch (severity)
        {
            case LogSeverity::Trace:
            case LogSeverity::Info:
                return "\x1b[38;5;32m";
            case LogSeverity::Warn:
                return "\x1b[38;5;214m";
            case LogSeverity::Debug:
                return "\x1b[38;5;40m";
            case LogSeverity::Error:
                return "\x1b[38;5;1m";
            case LogSeverity::Fatal:
                return "\x1b[48;5;1m";
        }
        return "\x1b[38;5;8m";
    }

    const char* Logger::GetSeverity(LogSeverity severity)
    {
        switch (severity)
        {
            case LogSeverity::Info:
                return "I";
            case LogSeverity::Warn:
                return "W";
            case LogSeverity::Debug:
                return "D";
            case LogSeverity::Error:
                return "E";
            case LogSeverity::Fatal:
                return "F";
            case LogSeverity::Trace:
                return "V";
        }
        return "D";
    }

    void Logger::RegisterWriter(StringID name, const SharedPtr<LoggerWriter>& writer)
    {
        s_writers[name] = writer;
    }

    void Logger::UnregisterWriter(StringID name)
    {
        s_writers.Remove(name);
    }

    void Logger::Log(LogSeverity severity,
                     const char* channel,
                     const char* file,
                     uint32_t line,
                     const char* format,
                     ...)
    {
        auto now = std::chrono::system_clock::now();
        auto nowSec = std::chrono::floor<std::chrono::seconds>(now);
        std::string timestamp = std::format("{:%Y-%m-%d %H:%M:%S}", nowSec);

        va_list args;
        va_start(args, format);

        String message = Wl::Formatv(format, args);

        for (auto [_, writer]: s_writers)
        {
            writer->Write(timestamp.c_str(), severity, channel, file, line, message.GetData());
        }

        va_end(args);
    }

}// namespace Wl