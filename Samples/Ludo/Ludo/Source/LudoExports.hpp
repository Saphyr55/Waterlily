#pragma once

#ifdef WL_LUDO_EXPORTS
#ifdef _MSC_VER
#define LUDO_API __declspec(dllexport)
#else
#define LUDO_API __attribute__((visibility("default")))
#endif
#else
#ifdef _MSC_VER
#define LUDO_API __declspec(dllimport)
#else
#define LUDO_API
#endif
#endif