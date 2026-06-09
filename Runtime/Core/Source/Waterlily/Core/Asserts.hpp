#pragma once

#include "Waterlily/Core/Defines.hpp"

#ifdef WL_DEBUG
    #define WL_CHECK(expr)   \
        WL_ENCAPULSE_BEGIN   \
        if (!(expr))         \
        {                    \
            WL_DEBUGBREAK(); \
        }                    \
        WL_ENCAPULSE_END
#else
    #define WL_CHECK(expr) ((void)(expr))
#endif
