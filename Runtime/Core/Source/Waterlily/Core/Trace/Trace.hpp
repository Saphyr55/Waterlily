#pragma once

#include "Waterlily/Core/Defines.hpp"
#include "Waterlily/Core/String/Format.hpp"
#include "Waterlily/Core/Trace/Logger.hpp"

#define LMAKE_CONTEXT_LOG_MSG(severity, channel, msg) \
    ::Wl::LogMessage(severity, channel, msg, uint32_t(__LINE__), __FUNCTION__, __FILE__, false)

#define LLOG(severity, channel, msg) ::Wl::Logger::GetDefault().Log(LMAKE_CONTEXT_LOG_MSG(severity, channel, msg));

#define LLOG_DEBUG(channel, msg) LLOG(::Wl::LogSeverity::Debug, channel, msg)

#define LLOG_INFO(channel, msg) LLOG(::Wl::LogSeverity::Info, channel, msg)

#define LLOG_WARN(channel, msg) LLOG(::Wl::LogSeverity::Warn, channel, msg)

#define LLOG_ERROR(channel, msg) LLOG(Wl::LogSeverity::Error, channel, msg)

#define LLOG_FATAL(channel, msg)                 \
    LLOG(::Wl::LogSeverity::Fatal, channel, msg) \
    LW_DEBUGBREAK();

#define LLOG_INFO_WHEN(cond, channel, msg) \
    if ((cond))                            \
    {                                      \
        LLOG_INFO(channel, msg)            \
    }

#define LLOG_FATAL_WHEN(cond, channel, msg) \
    if ((cond))                             \
    {                                       \
        LLOG_FATAL(channel, msg)            \
    }

#define LLOG_ERROR_WHEN(cond, channel, msg) \
    if ((cond))                             \
    {                                       \
        LLOG_ERROR(channel, msg)            \
    }

#define LLOG_WARN_WHEN(cond, channel, msg) \
    if ((cond))                            \
    {                                      \
        LLOG_WARN(channel, msg)            \
    }

#define LLOG_DEBUG_WHEN(cond, channel, msg) \
    if ((cond))                             \
    {                                       \
        LLOG_DEBUG(channel, msg)            \
    }

#define LLOG_ERROR_AND_RETURN_WHEN(expr, channel, msg) \
    if ((expr))                                        \
    {                                                  \
        LLOG_ERROR(channel, msg)                       \
        return;                                        \
    }

#define LLOG_ERROR_AND_RETURN_OBJECT_WHEN(expr, object, channel, msg) \
    if ((expr))                                                       \
    {                                                                 \
        LLOG_ERROR(channel, msg)                                      \
        return object;                                                \
    }

#define WL_ERROR_AND_RETURN_FALSE_WHEN(expr, channel, msg) \
    LLOG_ERROR_AND_RETURN_OBJECT_WHEN(expr, false, channel, msg)
