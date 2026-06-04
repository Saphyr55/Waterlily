#pragma once

#ifdef WL_ENTITY_EXPORTS
#ifdef _MSC_VER
#define WL_ENTITY_API __declspec(dllexport)
#else
#define WL_ENTITY_API __attribute__((visibility("default")))
#endif
#else
#ifdef _MSC_VER
#define WL_ENTITY_API __declspec(dllimport)
#else
#define WL_ENTITY_API
#endif
#endif