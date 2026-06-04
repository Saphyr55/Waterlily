#pragma once

#ifdef WL_RHI_EXPORTS
#ifdef _MSC_VER
#define WL_RHI_API __declspec(dllexport)
#else
#define WL_RHI_API __attribute__((visibility("default")))
#endif
#else
#ifdef _MSC_VER
#define WL_RHI_API __declspec(dllimport)
#else
#define WL_RHI_API
#endif
#endif
