#pragma once

#ifdef WL_SCENE_EXPORTS
#ifdef _MSC_VER
#define WL_SCENE_API __declspec(dllexport)
#else
#define WL_SCENE_API __attribute__((visibility("default")))
#endif
#else
#ifdef _MSC_VER
#define WL_SCENE_API __declspec(dllimport)
#else
#define WL_SCENE_API
#endif
#endif