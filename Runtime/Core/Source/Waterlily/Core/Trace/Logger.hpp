#pragma once

#include "Waterlily/Core/Containers/Array.hpp"
#include "Waterlily/Core/Containers/HashMap.hpp"
#include "Waterlily/Core/CoreExports.hpp"
#include "Waterlily/Core/Defines.hpp"
#include "Waterlily/Core/Function/Function.hpp"
#include "Waterlily/Core/String/StringRef.hpp"

namespace Wl
{

    /**
 * @enum LogSeverity
 * @brief Defines the severity levels for log messages.
 */
    enum class LogSeverity : int8_t
    {
        Info,
        Warn,
        Debug,
        Error,
        Fatal,
    };

    /**
 * @struct LogMessage
 * @brief Represents a log message with associated metadata.
 */
    struct WL_CORE_API LogMessage
    {
        /** Severity level of the log message. */
        LogSeverity Severity = LogSeverity::Debug;

        /** Channel or category of the log message. */
        StringRef Channel = "UNKNOWN";

        /** The actual log message. */
        StringRef Message = "NO MESSAGE";

        /** Line number where the log message was generated. */
        uint32_t Line = UINT32_MAX;

        /** Function name where the log message was generated. */
        StringRef Function = "UNKNOWN";

        /** File name where the log message was generated. */
        StringRef File = "UNKNOWN";

        /** Flag indicating whether the log message is verbose. */
        bool IsVerbose = false;

        LogMessage() = default;
        LogMessage(const LogSeverity severity,
                   const StringRef channel,
                   const StringRef message,
                   const uint32_t line,
                   const StringRef Function,
                   const StringRef file,
                   const bool verbose)
            : Severity(severity)
            , Channel(channel)
            , Message(message)
            , Line(line)
            , Function(Function)
            , File(file)
            , IsVerbose(verbose)
        {
        }

        ~LogMessage() = default;
    };

    /**
 * @typedef LogFn
 * @brief Function type for logging callbacks.
 */
    using LogDelegate = Function<void(const LogMessage& message)>;

    /**
 * @class Logger
 * @brief Provides a logging interface with a delegate function.
 */
    class WL_CORE_API Logger
    {
    public:
        /**
     * @brief Gets the default logger instance.
     * @return Reference to the default logger.
     */
        static Logger& GetDefault();

        static HashMap<LogSeverity, StringRef>& GetSeverityMap();

        /**
     * @brief Logs a message using the registered delegate.
     * @param message The log message to be logged.
     */
        void Log(const LogMessage& message) const;

        /**
     * @brief Gets the current logging delegate.
     * @return The current logging function.
     */
        LogDelegate GetDelegate() const;

        /**
     * @brief Sets the logging delegate.
     * @param func The logging function to set as the delegate.
     */
        void SetDelegate(const LogDelegate& func);

        /**
     * @brief Constructs a Logger with a specified logging function.
     * @param fn The logging function to use as the delegate.
     */
        explicit Logger(const LogDelegate& fn);

        /**
     * @brief Destructor.
     */
        ~Logger() = default;

    private:
        /** The logging delegate function. */
        LogDelegate m_logDelegate;
    };

    /**
 * @class ConsoleLogger
 * @brief Provides a logger that outputs messages to the console.
 */
    class WL_CORE_API ConsoleLogger
    {
    public:
        /**
     * @brief Gets the logging delegate for console output.
     * @return The logging function for console output.
     */
        LogDelegate GetDelegate();

        /**
     * @brief Constructs a ConsoleLogger.
     */
        ConsoleLogger();

        /**
     * @brief Destructor.
     */
        ~ConsoleLogger() = default;

    private:
        /** The logging function for console output. */
        LogDelegate m_consoleLog;
    };

    /**
 * @class CompositeLogger
 * @brief Combines multiple loggers into a single logger.
 */
    class WL_CORE_API CompositeLogger
    {
    public:
        /**
     * @brief Adds a logging function to the composite logger.
     * @param logger The logging function to add.
     */
        void AddLogDelegate(LogDelegate logger);

        /**
     * @brief Logs a message using all registered logging functions.
     * @param message The log message to be logged.
     */
        void Log(const LogMessage& message) const;

    private:
        /** Collection of registered logging functions. */
        Array<LogDelegate> m_loggers;
    };

}// namespace Wl