#pragma once

#include <cstddef>
#include <cstdint>
#include <cstdlib>
#include <iostream>

using TypeIndex = size_t;

inline TypeIndex GetTypeIndex()
{
    static TypeIndex s_next_type_index = 0;
    return s_next_type_index++;
}

template<typename T>
inline TypeIndex GetTypeIndex()
{
    static TypeIndex s_type_index = GetTypeIndex();
    return s_type_index;
}

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
    #define LW_DEBUGBREAK() __debugbreak()
#else
    #define LW_DEBUGBREAK() __builtin_trap()
#endif

#ifdef _WIN32
    #undef min
    #undef max
    #undef ERROR
    #undef DELETE
    #undef MessageBox
    #undef Error
    #undef OK
    #undef CONNECT_DEFERRED
    #undef MONO_FONT
    #define NOGDI
#endif

/**
 *
 */
#ifdef WL_DEBUG
    #define WL_CHECK(expr)       \
        do                       \
        {                        \
            if (!(expr))         \
            {                    \
                LW_DEBUGBREAK(); \
            }                    \
        } while (false)
#else
    #define WL_CHECK(expr) ((void)(expr))
#endif

/**
 *
 */
// TODO: Remove std::cout ...
#ifdef WL_DEBUG
    #define WL_CHECK_MSG(expr, msg)         \
        do                                  \
        {                                   \
            if (!(expr))                    \
            {                               \
                ::std::cout << msg << "\n"; \
                LW_DEBUGBREAK();            \
            }                               \
        } while (false)
#else
    #define WL_CHECK_MSG(expr, msg) ((void)(expr))
#endif

#define WL_DEPRECATED(version, msg)

#define WL_CRASH(msg)         \
    WL_CHECK_MSG(false, msg); \
    abort()

#define WL_CONCAT_INNER(a, b) a##b
#define WL_CONCAT(a, b) WL_CONCAT_INNER(a, b)

#define WL_ENUM_FLAGS_CUSTOM_DERIVED(EnumClassName, IntType)                                  \
    inline EnumClassName operator|(EnumClassName a, EnumClassName b)                          \
    {                                                                                         \
        return static_cast<EnumClassName>(static_cast<IntType>(a) | static_cast<IntType>(b)); \
    }                                                                                         \
    inline EnumClassName operator&(EnumClassName a, EnumClassName b)                          \
    {                                                                                         \
        return static_cast<EnumClassName>(static_cast<IntType>(a) & static_cast<IntType>(b)); \
    }                                                                                         \
    inline EnumClassName& operator|=(EnumClassName& a, EnumClassName b)                       \
    {                                                                                         \
        a = a | b;                                                                            \
        return a;                                                                             \
    }                                                                                         \
    inline EnumClassName& operator&=(EnumClassName& a, EnumClassName b)                       \
    {                                                                                         \
        a = a & b;                                                                            \
        return a;                                                                             \
    }

#define WL_ENUM_FLAGS(EnumClassName) WL_ENUM_FLAGS_CUSTOM_DERIVED(EnumClassName, uint32_t)

#define WL_RETURN_WHEN(expr) \
    if ((expr))              \
    {                        \
        return;              \
    }

#define WL_RETURN_OBJECT_WHEN(expr, object) \
    if ((expr))                             \
    {                                       \
        return object;                      \
    }

#define WL_RETURN_FALSE_WHEN(expr) WL_RETURN_OBJECT_WHEN(expr, false)

#define WL_RETURN_TRUE_WHEN(expr) WL_RETURN_OBJECT_WHEN(expr, true)
