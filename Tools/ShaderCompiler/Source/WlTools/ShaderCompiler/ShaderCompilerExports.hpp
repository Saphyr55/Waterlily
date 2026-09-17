#pragma once

#ifdef WL_TOOLS_SHADER_COMPILER_EXPORTS
    #ifdef _MSC_VER
        #define WL_TOOLS_SHADER_COMPILER_API __declspec(dllexport)
    #else
        #define WL_TOOLS_SHADER_COMPILER_API __attribute__((visibility("default")))
    #endif
#else
    #ifdef _MSC_VER
        #define WL_TOOLS_SHADER_COMPILER_API __declspec(dllimport)
    #else
        #define WL_TOOLS_SHADER_COMPILER_API
    #endif
#endif