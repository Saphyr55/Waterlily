#include "Waterlily/Core/Trace/Logger.hpp"
#include "Waterlily/Core/String/StringRef.hpp"

#include <iostream>

namespace Wl
{

    HashMap<LogSeverity, StringRef>& Logger::GetSeverityMap()
    {
        static HashMap<LogSeverity, StringRef> s_severityMap = {{LogSeverity::Info, "I"},
                                                                {LogSeverity::Warn, "W"},
                                                                {LogSeverity::Error, "E"},
                                                                {LogSeverity::Fatal, "F"},
                                                                {LogSeverity::Debug, "D"}};

        return s_severityMap;
    }

    Logger& Logger::GetDefault()
    {
        static Logger s_logger = [&]()
        {
            static ConsoleLogger consoleLogger;
            static CompositeLogger compisteLogger;
            compisteLogger.AddLogDelegate(consoleLogger.GetDelegate());
            return Logger([&](const LogMessage& message)
            {
                compisteLogger.Log(message);
            });
        }();

        return s_logger;
    }

    void Logger::Log(const LogMessage& message) const
    {
        m_logDelegate(message);
    }

    LogDelegate Logger::GetDelegate() const
    {
        return m_logDelegate;
    }

    void Logger::SetDelegate(const LogDelegate& delegate)
    {
        m_logDelegate = delegate;
    }

    Logger::Logger(const LogDelegate& delegate)
        : m_logDelegate(delegate)
    {
    }

    ConsoleLogger::ConsoleLogger()
        : m_consoleLog([](const LogMessage& message) -> void
    {
        const char* severity_str = Logger::GetSeverityMap()[message.Severity];

        if (!severity_str)
        {
            severity_str = "UNKNOWN";
        }

        std::cout << "[" << severity_str << "] " << message.Channel << " " << message.Message << "\n";
    })
    {
    }

    LogDelegate ConsoleLogger::GetDelegate()
    {
        return m_consoleLog;
    }

    void CompositeLogger::AddLogDelegate(LogDelegate logger)
    {
        m_loggers.Append(logger);
    }

    void CompositeLogger::Log(const LogMessage& message) const
    {
        for (const LogDelegate& logger: m_loggers)
        {
            logger(message);
        }
    }

}// namespace Wl