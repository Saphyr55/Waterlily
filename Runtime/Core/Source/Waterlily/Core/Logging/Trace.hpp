#pragma once

#include "Waterlily/Core/Defines.hpp"
#include "Waterlily/Core/Logging/Logger.hpp"

#define WL_LOG(severity, channel, format, ...) ::Wl::Logger::Log(severity, channel, __FILE__, __LINE__, format, ##__VA_ARGS__)

#define WL_LOG_DEBUG(channel, format, ...) WL_LOG(::Wl::LogSeverity::Debug, channel, format, ##__VA_ARGS__)

#define WL_LOG_INFO(channel, format, ...) WL_LOG(::Wl::LogSeverity::Info, channel, format, ##__VA_ARGS__)

#define WL_LOG_WARN(channel, format, ...) WL_LOG(::Wl::LogSeverity::Warn, channel, format, ##__VA_ARGS__)

#define WL_LOG_ERROR(channel, format, ...) WL_LOG(Wl::LogSeverity::Error, channel, format, ##__VA_ARGS__)

#define WL_LOG_FATAL(channel, format, ...)                            \
    WL_ENCAPULSE_BEGIN                                             \
    WL_LOG(::Wl::LogSeverity::Fatal, channel, format, ##__VA_ARGS__); \
    WL_DEBUGBREAK();                                               \
    WL_ENCAPULSE_END

#define WL_LOG_INFO_WHEN(cond, channel, format, ...) \
    WL_ENCAPULSE_BEGIN                            \
    if ((cond))                                   \
    {                                             \
        WL_LOG_INFO(channel, format, ##__VA_ARGS__); \
    }                                             \
    WL_ENCAPULSE_END

#define WL_LOG_FATAL_WHEN(cond, channel, format, ...) \
    WL_ENCAPULSE_BEGIN                             \
    if ((cond))                                    \
    {                                              \
        WL_LOG_FATAL(channel, format, ##__VA_ARGS__); \
    }                                              \
    WL_ENCAPULSE_END

#define WL_LOG_ERROR_WHEN(cond, channel, format, ...) \
    WL_ENCAPULSE_BEGIN                             \
    if ((cond))                                    \
    {                                              \
        WL_LOG_ERROR(channel, format, ##__VA_ARGS__); \
    }                                              \
    WL_ENCAPULSE_END

#define WL_LOG_WARN_WHEN(cond, channel, format, ...) \
    WL_ENCAPULSE_BEGIN                            \
    if ((cond))                                   \
    {                                             \
        WL_LOG_WARN(channel, format, ##__VA_ARGS__); \
    }                                             \
    WL_ENCAPULSE_END

#define WL_DEBUG_WHEN(cond, channel, format, ...)     \
    WL_ENCAPULSE_BEGIN                             \
    if ((cond))                                    \
    {                                              \
        WL_LOG_DEBUG(channel, format, ##__VA_ARGS__); \
    }                                              \
    WL_ENCAPULSE_END

#ifdef WL_DEBUG
    #define WL_CHECK_MSG(expr, format, ...)                             \
        WL_ENCAPULSE_BEGIN                                           \
        WL_LOG_FATAL_WHEN(!(expr), "[ASSERTS]", format, ##__VA_ARGS__); \
        WL_ENCAPULSE_END
#else
    #define WL_CHECK_MSG(expr, format, ...) ((void)(expr))
#endif

#define WL_CRASH(format, ...)                   \
    WL_ENCAPULSE_BEGIN                       \
    WL_CHECK_MSG(false, format, ##__VA_ARGS__); \
    WL_ABORT();                              \
    WL_ENCAPULSE_END
