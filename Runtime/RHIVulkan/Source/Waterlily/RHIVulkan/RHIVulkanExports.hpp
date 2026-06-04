#pragma once

#ifdef WL_RHI_VULKAN_EXPORTS
#ifdef _MSC_VER
#define WL_RHI_VULKAN_API __declspec(dllexport)
#else
#define WL_RHI_VULKAN_API __attribute__((visibility("default")))
#endif
#else
#ifdef _MSC_VER
#define WL_RHI_VULKAN_API __declspec(dllimport)
#else
#define WL_RHI_VULKAN_API
#endif
#endif
