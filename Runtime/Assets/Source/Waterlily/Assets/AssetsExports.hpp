#pragma once

#ifdef WL_ASSETS_EXPORTS
    #ifdef _MSC_VER
        #define WL_ASSETS_API __declspec(dllexport)
    #else
        #define WL_ASSETS_API __attribute__((visibility("default")))
    #endif
#else
    #ifdef _MSC_VER
        #define WL_ASSETS_API __declspec(dllimport)
    #else
        #define WL_ASSETS_API
    #endif
#endif