#pragma once

#ifdef WL_LUDO_DEV_EXPORTS
    #ifdef _MSC_VER
        #define LUDO_DEV_API __declspec(dllexport)
    #else
        #define LUDO_DEV_API __attribute__((visibility("default")))
    #endif
#else
    #ifdef _MSC_VER
        #define LUDO_DEV_API __declspec(dllimport)
    #else
        #define LUDO_DEV_API
    #endif
#endif
