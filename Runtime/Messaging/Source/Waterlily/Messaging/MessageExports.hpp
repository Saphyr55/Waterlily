#pragma once

#ifdef WL_MESSAGING_EXPORTS
#ifdef _MSC_VER
#define WL_MESSAGING_API __declspec(dllexport)
#else
#define WL_MESSAGING_API __attribute__((visibility("default")))
#endif
#else
#ifdef _MSC_VER
#define WL_MESSAGING_API __declspec(dllimport)
#else
#define WL_MESSAGING_API
#endif
#endif
