#pragma once

#include "Waterlily/Core/Defines.hpp"
#include "Waterlily/Core/String/Format.hpp"
#include "Waterlily/Core/Trace/Logger.hpp"

#define LMAKE_CONTEXT_LOG_MSG(severity, channel, msg) \
    ::Wl::LogMessage(severity, channel, msg, uint32_t(__LINE__), __FUNCTION__, __FILE__, false)

#define WL_LOG(severity, channel, msg) ::Wl::Logger::GetDefault().Log(LMAKE_CONTEXT_LOG_MSG(severity, channel, msg));

#define WL_LOG_DEBUG(channel, msg) WL_LOG(::Wl::LogSeverity::Debug, channel, msg)

#define WL_LOG_INFO(channel, msg) WL_LOG(::Wl::LogSeverity::Info, channel, msg)

#define WL_LOG_WARN(channel, msg) WL_LOG(::Wl::LogSeverity::Warn, channel, msg)

#define WL_LOG_ERROR(channel, msg) WL_LOG(Wl::LogSeverity::Error, channel, msg)

#define WL_LOG_FATAL(channel, msg)                 \
    WL_LOG(::Wl::LogSeverity::Fatal, channel, msg) \
    LW_DEBUGBREAK();

#define WL_LOG_INFO_WHEN(cond, channel, msg) \
    if ((cond))                            \
    {                                      \
        WL_LOG_INFO(channel, msg)            \
    }

#define WL_LOG_FATAL_WHEN(cond, channel, msg) \
    if ((cond))                             \
    {                                       \
        WL_LOG_FATAL(channel, msg)            \
    }

#define WL_LOG_ERROR_WHEN(cond, channel, msg) \
    if ((cond))                             \
    {                                       \
        WL_LOG_ERROR(channel, msg)            \
    }

#define WL_LOG_WARN_WHEN(cond, channel, msg) \
    if ((cond))                            \
    {                                      \
        WL_LOG_WARN(channel, msg)            \
    }

#define WL_DEBUG_WHEN(cond, channel, msg) \
    if ((cond))                             \
    {                                       \
        WL_LOG_DEBUGchannel, msg)            \
    }

#define WL_LOG_ERROR_AND_RETURN_WHEN(expr, channel, msg) \
    if ((expr))                                        \
    {                                                  \
        WL_LOG_ERROR(channel, msg)                       \
        return;                                        \
    }

#define WL_LOG_ERROR_AND_RETURN_OBJECT_WHEN(expr, object, channel, msg) \
    if ((expr))                                                       \
    {                                                                 \
        WL_LOG_ERROR(channel, msg)                                      \
        return object;                                                \
    }

#define WL_LOG_ERROR_AND_RETURN_FALSE_WHEN(expr, channel, msg) \
    WL_LOG_ERROR_AND_RETURN_OBJECT_WHEN(expr, false, channel, msg)
