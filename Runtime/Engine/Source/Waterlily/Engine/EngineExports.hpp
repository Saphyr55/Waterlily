#pragma once

#ifdef WL_ENGINE_EXPORTS
    #ifdef _MSC_VER
        #define WL_ENGINE_API __declspec(dllexport)
    #else
        #define WL_ENGINE_API __attribute__((visibility("default")))
    #endif
#else
    #ifdef _MSC_VER
        #define WL_ENGINE_API __declspec(dllimport)
    #else
        #define WL_ENGINE_API
    #endif
#endif