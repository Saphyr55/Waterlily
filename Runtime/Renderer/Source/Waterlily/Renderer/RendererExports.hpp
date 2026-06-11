#pragma once

#ifdef WL_RENDERER_EXPORTS
    #ifdef _MSC_VER
        #define WL_RENDERER_API __declspec(dllexport)
    #else
        #define WL_RENDERER_API __attribute__((visibility("default")))
    #endif
#else
    #ifdef _MSC_VER
        #define WL_RENDERER_API __declspec(dllimport)
    #else
        #define WL_RENDERER_API
    #endif
#endif