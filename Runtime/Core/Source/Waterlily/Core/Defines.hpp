#pragma once

#include <cstddef>
#include <cstdint>
#include <cstdlib>
#include <iostream>

/**
 * Defines WL_DEBUG on debugging.
 */
#ifndef WL_DEBUG
    #if defined(_DEBUG) || defined(DEBUG)
        #define WL_DEBUG
    #endif
#endif

/**
 *
 */
#if defined(_MSC_VER)
    #define WL_DEBUGBREAK() __debugbreak()
#else
    #define WL_DEBUGBREAK() __builtin_trap()
#endif

#ifdef _WIN32
    #undef min
    #undef max
    #undef ERROR
    #undef DELETE
    #undef MessageBox
    #undef CreateSemaphore
    #undef Error
    #undef OK
    #undef CONNECT_DEFERRED
    #undef MONO_FONT
    #define NOGDI
#endif


#define WL_DEPRECATED(version, msg)

#define WL_TOSTRING(value) #value

#define WL_ABORT(...) abort(##__VA_ARGS__)

#define WL_CONCAT_INNER(a, b) a##b
#define WL_CONCAT(a, b) WL_CONCAT_INNER(a, b)

#define WL_ENUM_FLAGS_CUSTOM_DERIVED(EnumClassName, IntType)                                  \
    constexpr inline EnumClassName operator|(EnumClassName a, EnumClassName b)                \
    {                                                                                         \
        return static_cast<EnumClassName>(static_cast<IntType>(a) | static_cast<IntType>(b)); \
    }                                                                                         \
    constexpr inline EnumClassName operator&(EnumClassName a, EnumClassName b)                \
    {                                                                                         \
        return static_cast<EnumClassName>(static_cast<IntType>(a) & static_cast<IntType>(b)); \
    }                                                                                         \
    constexpr inline EnumClassName& operator|=(EnumClassName& a, EnumClassName b)             \
    {                                                                                         \
        a = a | b;                                                                            \
        return a;                                                                             \
    }                                                                                         \
    constexpr inline EnumClassName& operator&=(EnumClassName& a, EnumClassName b)             \
    {                                                                                         \
        a = a & b;                                                                            \
        return a;                                                                             \
    }                                                                                         \
    constexpr inline EnumClassName operator~(EnumClassName a)                                 \
    {                                                                                         \
        return static_cast<EnumClassName>(~static_cast<IntType>(a));                          \
    }

#define WL_ENUM_FLAGS(EnumClassName) WL_ENUM_FLAGS_CUSTOM_DERIVED(EnumClassName, uint32_t)

#define WL_ENCAPULSE_BEGIN \
    do                     \
    {

#define WL_ENCAPULSE_END \
    }                    \
    while (false)

#define WL_RETURN_WHEN(expr) \
    WL_ENCAPULSE_BEGIN       \
    if ((expr))              \
    {                        \
        return;              \
    }                        \
    WL_ENCAPULSE_END

#define WL_RETURN_OBJECT_WHEN(expr, object) \
    WL_ENCAPULSE_BEGIN                      \
    if ((expr))                             \
    {                                       \
        return object;                      \
    }                                       \
    WL_ENCAPULSE_END


#define WL_RETURN_FALSE_WHEN(expr) WL_RETURN_OBJECT_WHEN(expr, false)

#define WL_RETURN_TRUE_WHEN(expr) WL_RETURN_OBJECT_WHEN(expr, true)

#ifdef __clang__
    #define WL_PRETTY_FUNCTION() __PRETTY_FUNCTION__
#elif defined(__GNUC__)
    #define WL_PRETTY_FUNCTION() __PRETTY_FUNCTION__
#elif defined(_MSC_VER)
    #define WL_PRETTY_FUNCTION() __FUNCSIG__
#else
    #error "Unsupported Compiler"
#endif
