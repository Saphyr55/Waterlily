#include "Waterlily/RHIVulkan/VulkanContext.hpp"

#include "Waterlily/Core/Function/FunctionRef.hpp"
#include "Waterlily/Core/Memory/SharedPtr.hpp"
#include "Waterlily/Core/Platform/DynamicLibrary.hpp"
#include "Waterlily/Core/Trace/Trace.hpp"
#include "Waterlily/RHI/Types.hpp"
#include "Waterlily/RHIVulkan/VulkanCommandQueue.hpp"
#include "Waterlily/RHIVulkan/VulkanLoader.hpp"
#include "Waterlily/RHIVulkan/VulkanPhysicalDevice.hpp"
#include "Waterlily/RHIVulkan/VulkanRenderSurface.hpp"
#include "Waterlily/RHIVulkan/VulkanShaderModule.hpp"

#include <vk_mem_alloc.h>

namespace Wl
{

    static VulkanContext g_context;

    VulkanContext& VulkanContextGet()
    {
        return g_context;
    }

    void VulkanDeviceCreate(VulkanContext& context, VulkanPhysicalDeviceSelector& physicalDeviceSelector)
    {
        physicalDeviceSelector.SelectPhysicalDevice();
        context.PhysicalDeviceInfo = physicalDeviceSelector.GetInfo();

        WL_CHECK_MSG(context.PhysicalDeviceInfo.IsSuitable, "Physical device is not suitable for Vulkan operations.");

        float queuePriority = 1.0f;

        Array<uint32_t> queueFamillies = VulkanQueryQueueFamilyIndices(context);

        WL_LOG_INFO("[Vulkan]", "Creating Vulkan logical device with the following queue families:");
        Array<VkDeviceQueueCreateInfo> deviceQueueCreateInfos;
        for (uint32_t queueFamilyIndex: queueFamillies)
        {
            VkDeviceQueueCreateInfo createInfo = {};
            createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
            createInfo.pNext = VK_NULL_HANDLE;
            createInfo.queueFamilyIndex = queueFamilyIndex;
            createInfo.queueCount = 1;
            createInfo.pQueuePriorities = &queuePriority;
            deviceQueueCreateInfos.Append(createInfo);

            const VkQueueFamilyProperties& props = context.PhysicalDeviceInfo.QueueFamilies[queueFamilyIndex];
            String typeStr(20);
            if (props.queueFlags & VK_QUEUE_GRAPHICS_BIT)
            {
                typeStr += "Graphics ";
            }
            if (props.queueFlags & VK_QUEUE_COMPUTE_BIT)
            {
                typeStr += "Compute ";
            }
            if (props.queueFlags & VK_QUEUE_TRANSFER_BIT)
            {
                typeStr += "Transfer ";
            }
            if (props.queueFlags & VK_QUEUE_SPARSE_BINDING_BIT)
            {
                typeStr += "Sparse ";
            }

            bool presentSupport = VulkanQueuePresentModeIsSupported(context, queueFamilyIndex);
            WL_LOG_INFO("[Vulkan]",
                      Wl::Format("Queue Family Index: %d | Queues: %d | Types: %s| Present: %s",
                              queueFamilyIndex,
                              props.queueCount,
                              typeStr.GetData(),
                              presentSupport ? "Yes" : "No"));
        }

        VkPhysicalDeviceFeatures physicalDeviceFeatures = physicalDeviceSelector.GetInfo().Features;
        VkPhysicalDeviceVulkan12Features physicalDevice12Features =
                physicalDeviceSelector.GetInfo().VulkanFeatures12;

        Array<const char*> physicalDeviceExtensions;
        physicalDeviceExtensions.Resize(s_PhysicalDeviceExtensions.size());

        for (int32_t i = 0; i < s_PhysicalDeviceExtensions.size(); i++)
        {
            physicalDeviceExtensions[i] = s_PhysicalDeviceExtensions[i].data();
        }

        VkDeviceCreateInfo deviceCreateInfo = {};
        deviceCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
        deviceCreateInfo.pNext = &physicalDevice12Features;
        deviceCreateInfo.pQueueCreateInfos = deviceQueueCreateInfos.data();
        deviceCreateInfo.queueCreateInfoCount = static_cast<uint32_t>(deviceQueueCreateInfos.size());
        deviceCreateInfo.pEnabledFeatures = &physicalDeviceFeatures;
        deviceCreateInfo.ppEnabledExtensionNames = physicalDeviceExtensions.data();
        deviceCreateInfo.enabledExtensionCount = static_cast<uint32_t>(physicalDeviceExtensions.size());

        WL_LOG_INFO("[Vulkan]", "Enabled Physical Device Extensions:");
        for (int32_t i = 0; i < physicalDeviceExtensions.size(); i++)
        {
            WL_LOG_INFO("[Vulkan]", Wl::Format("Extension: %s", physicalDeviceExtensions[i]));
        }

        WL_VULKAN_CHECK(
                VulkanAPI::vkCreateDevice(context.PhysicalDevice, &deviceCreateInfo, context.Allocator, &context.Device));

        VulkanDeviceAPILoad(context.Device);

        VmaVulkanFunctions vmaVulkanFunctions = {};
        vmaVulkanFunctions.vkGetInstanceProcAddr = VulkanAPI::vkGetInstanceProcAddr;
        vmaVulkanFunctions.vkGetDeviceProcAddr = VulkanAPI::vkGetDeviceProcAddr;

        VmaAllocatorCreateInfo vmaAllocatorCreateInfo = {};
        vmaAllocatorCreateInfo.vulkanApiVersion = context.VulkanAPIVersion;
        vmaAllocatorCreateInfo.instance = context.Instance;
        vmaAllocatorCreateInfo.physicalDevice = context.PhysicalDevice;
        vmaAllocatorCreateInfo.device = context.Device;
        vmaAllocatorCreateInfo.pAllocationCallbacks = context.Allocator;
        vmaAllocatorCreateInfo.pVulkanFunctions = &vmaVulkanFunctions;
        vmaAllocatorCreateInfo.flags =
                VMA_ALLOCATOR_CREATE_EXT_MEMORY_BUDGET_BIT | VMA_ALLOCATOR_CREATE_EXT_MEMORY_PRIORITY_BIT;

        vmaCreateAllocator(&vmaAllocatorCreateInfo, &context.VmaAllocator);
    }

    void VulkanDeviceDestroy(VulkanContext& context)
    {
        WL_LOG_INFO("[Vulkan]", "Destroying Vulkan logical device.");

        VulkanAPI::vkDestroyDevice(context.Device, context.Allocator);
        context.Device = VK_NULL_HANDLE;

        WL_LOG_INFO("[Vulkan]", "Vulkan logical device destroyed.");
    }

    void VulkanContextCreate(VulkanContext& context, void* native_window)
    {
        WL_LOG_INFO("[Vulkan]", "Initializing Vulkan RHI context...");
        {
            context.Library = VulkanLibraryLoad();
            WL_LOG_FATAL_WHEN(!context.Library, "[Vulkan]", "Failed to load Vulkan RHI library.");

            bool isCoreFunctionsLoaded = VulkanCoreAPILoad(context.Library);
            WL_LOG_FATAL_WHEN(!isCoreFunctionsLoaded, "[Vulkan]", "Failed to load Vulkan RHI core functions.");

            VulkanInstanceCreate(context);

            VulkanDebugMessengerCreate(context);

            context.Surface = VulkanRenderSurface::Create(context, native_window);
            context.Surface->Create();

            WL_CHECK(context.Surface->GetHandle());

            VulkanPhysicalDeviceSelector selector(context, s_PhysicalDeviceExtensions);
            VulkanDeviceCreate(context, selector);

            size_t familyQueueSize = context.PhysicalDeviceInfo.QueueFamilies.size();
            context.CommandQueues.Resize(familyQueueSize);

            for (size_t i = 0; i < familyQueueSize; i++)
            {
                VkQueue queue = VulkanQueryQueue(context, i, 0);
                RHIQueueType type = VulkanQueueType(context, i);
                bool isPresentMode = VulkanQueuePresentModeIsSupported(context, i);
                context.CommandQueues[i] = MakeShared<VulkanCommandQueue>(context, queue, type, i, isPresentMode);
            }
        }
    }

    void VulkanContextDestroy(VulkanContext& context)
    {
        VulkanDeviceDestroy(context);

        context.Surface->Destroy();

        VulkanDebugMessengerDestroy(context);

        VulkanInstanceDestroy(context);

        if (context.Library)
        {
            DynamicLibraryLoader::Unload(context.Library);
            WL_LOG_INFO("[Vulkan]", "Vulkan RHI context destroyed.");
        }
    }

    bool VulkanQueuePresentModeIsSupported(VulkanContext& context, uint32_t queueFamilyIndex)
    {
        VkBool32 isPresentModeSupported = false;
        WL_VULKAN_CHECK(VulkanAPI::vkGetPhysicalDeviceSurfaceSupportKHR(context.PhysicalDevice,
                                                                        queueFamilyIndex,
                                                                        context.Surface->GetHandle(),
                                                                        &isPresentModeSupported));
        return isPresentModeSupported;
    }

    RHIQueueType VulkanQueueType(VulkanContext& context, uint32_t queueFamilyIndex)
    {
        const VkQueueFamilyProperties& queue_family_properties =
                context.PhysicalDeviceInfo.QueueFamilies[queueFamilyIndex];

        if (queue_family_properties.queueFlags & VK_QUEUE_GRAPHICS_BIT)
        {
            return RHIQueueType::Graphics;
        }

        if (queue_family_properties.queueFlags & VK_QUEUE_COMPUTE_BIT)
        {
            return RHIQueueType::Compute;
        }

        if (queue_family_properties.queueFlags & VK_QUEUE_TRANSFER_BIT)
        {
            return RHIQueueType::Transfer;
        }

        return RHIQueueType::Unknown;
    }

    Array<uint32_t> VulkanQueryQueueFamilyIndices(VulkanContext& context,
                                                  FunctionRef<bool(const VkQueueFamilyProperties&, uint32_t)> predicate)
    {
        VulkanPhysicalDeviceInformation& info = context.PhysicalDeviceInfo;
        Array<uint32_t> indices(info.QueueFamilies.size());

        for (uint32_t queue_family_index = 0; queue_family_index < info.QueueFamilies.size(); queue_family_index++)
        {
            if (predicate(info.QueueFamilies[queue_family_index], queue_family_index))
            {
                indices.Append(queue_family_index);
            }
        }

        return indices;
    }

    VkQueue VulkanQueryQueue(VulkanContext& context, uint32_t queue_family_index, uint32_t queue_index)
    {
        VkQueue queue;
        VulkanAPI::vkGetDeviceQueue(context.Device, queue_family_index, queue_index, &queue);
        return queue;
    }

    const char* VulkanStringOfPresentMode(VkPresentModeKHR present_mode)
    {
        switch (present_mode)
        {
            case VK_PRESENT_MODE_IMMEDIATE_KHR:
                return "VK_PRESENT_MODE_IMMEDIATE_KHR";
            case VK_PRESENT_MODE_MAILBOX_KHR:
                return "VK_PRESENT_MODE_MAILBOX_KHR";
            case VK_PRESENT_MODE_FIFO_KHR:
                return "VK_PRESENT_MODE_FIFO_KHR";
            case VK_PRESENT_MODE_FIFO_RELAXED_KHR:
                return "VK_PRESENT_MODE_FIFO_RELAXED_KHR";
            case VK_PRESENT_MODE_SHARED_DEMAND_REFRESH_KHR:
                return "VK_PRESENT_MODE_SHARED_DEMAND_REFRESH_KHR";
            case VK_PRESENT_MODE_SHARED_CONTINUOUS_REFRESH_KHR:
                return "VK_PRESENT_MODE_SHARED_CONTINUOUS_REFRESH_KHR";
            default:
                return "VK_UNKNOWN_PRESENT_MODE";
        }
    }

    const char* VulkanStringOfResult(VkResult result)
    {
        switch (result)
        {
            case VK_SUCCESS:
                return "VK_SUCCESS";
            case VK_NOT_READY:
                return "VK_NOT_READY";
            case VK_TIMEOUT:
                return "VK_TIMEOUT";
            case VK_EVENT_SET:
                return "VK_EVENT_SET";
            case VK_EVENT_RESET:
                return "VK_EVENT_RESET";
            case VK_INCOMPLETE:
                return "VK_INCOMPLETE";
            case VK_ERROR_OUT_OF_HOST_MEMORY:
                return "VK_ERROR_OUT_OF_HOST_MEMORY";
            case VK_ERROR_OUT_OF_DEVICE_MEMORY:
                return "VK_ERROR_OUT_OF_DEVICE_MEMORY";
            case VK_ERROR_INITIALIZATION_FAILED:
                return "VK_ERROR_INITIALIZATION_FAILED";
            case VK_ERROR_DEVICE_LOST:
                return "VK_ERROR_DEVICE_LOST";
            case VK_ERROR_MEMORY_MAP_FAILED:
                return "VK_ERROR_MEMORY_MAP_FAILED";
            case VK_ERROR_LAYER_NOT_PRESENT:
                return "VK_ERROR_LAYER_NOT_PRESENT";
            case VK_ERROR_EXTENSION_NOT_PRESENT:
                return "VK_ERROR_EXTENSION_NOT_PRESENT";
            case VK_ERROR_FEATURE_NOT_PRESENT:
                return "VK_ERROR_FEATURE_NOT_PRESENT";
            case VK_ERROR_INCOMPATIBLE_DRIVER:
                return "VK_ERROR_INCOMPATIBLE_DRIVER";
            case VK_ERROR_TOO_MANY_OBJECTS:
                return "VK_ERROR_TOO_MANY_OBJECTS";
            case VK_ERROR_FORMAT_NOT_SUPPORTED:
                return "VK_ERROR_FORMAT_NOT_SUPPORTED";
            case VK_ERROR_FRAGMENTED_POOL:
                return "VK_ERROR_FRAGMENTED_POOL";
            case VK_ERROR_SURFACE_LOST_KHR:
                return "VK_ERROR_SURFACE_LOST_KHR";
            case VK_ERROR_NATIVE_WINDOW_IN_USE_KHR:
                return "VK_ERROR_NATIVE_WINDOW_IN_USE_KHR";
            case VK_SUBOPTIMAL_KHR:
                return "VK_SUBOPTIMAL_KHR";
            case VK_ERROR_OUT_OF_DATE_KHR:
                return "VK_ERROR_OUT_OF_DATE_KHR";
            case VK_ERROR_INCOMPATIBLE_DISPLAY_KHR:
                return "VK_ERROR_INCOMPATIBLE_DISPLAY_KHR";
            case VK_ERROR_VALIDATION_FAILED_EXT:
                return "VK_ERROR_VALIDATION_FAILED_EXT";
            case VK_ERROR_INVALID_SHADER_NV:
                return "VK_ERROR_INVALID_SHADER_NV";
            case VK_ERROR_INVALID_DRM_FORMAT_MODIFIER_PLANE_LAYOUT_EXT:
                return "VK_ERROR_INVALID_DRM_FORMAT_MODIFIER_PLANE_LAYOUT_EXT";
            default:
                return "VK_UNKNOWN_ERROR";
        }
    }

    RHIFormat RHIFormatGet(VkFormat format)
    {
        switch (format)
        {
            // 8-bit
            case VK_FORMAT_R8_UNORM:
                return RHIFormat::R8;
            case VK_FORMAT_R8G8_UNORM:
                return RHIFormat::RG8;
            case VK_FORMAT_R8G8B8_UNORM:
                return RHIFormat::RGB8;
            case VK_FORMAT_R8G8B8A8_UNORM:
                return RHIFormat::RGBA8;
            case VK_FORMAT_B8G8R8A8_UNORM:
                return RHIFormat::BGRA8;

            case VK_FORMAT_R8_SRGB:
                return RHIFormat::R8sRGB;
            case VK_FORMAT_R8G8_SRGB:
                return RHIFormat::RG8sRGB;
            case VK_FORMAT_R8G8B8_SRGB:
                return RHIFormat::RGB8sRGB;
            case VK_FORMAT_R8G8B8A8_SRGB:
                return RHIFormat::RGBA8sRGB;
            case VK_FORMAT_B8G8R8A8_SRGB:
                return RHIFormat::BGRA8sRGB;

            // 16-bit signed int
            case VK_FORMAT_R16_SINT:
                return RHIFormat::R16_SINT;
            case VK_FORMAT_R16G16_SINT:
                return RHIFormat::RG16_SINT;
            case VK_FORMAT_R16G16B16_SINT:
                return RHIFormat::RGB16_SINT;
            case VK_FORMAT_R16G16B16A16_SINT:
                return RHIFormat::RGBA16_SINT;
            // 16-bit unsigned int
            case VK_FORMAT_R16_UINT:
                return RHIFormat::R16_UINT;
            case VK_FORMAT_R16G16_UINT:
                return RHIFormat::RG16_UINT;
            case VK_FORMAT_R16G16B16_UINT:
                return RHIFormat::RGB16_UINT;
            case VK_FORMAT_R16G16B16A16_UINT:
                return RHIFormat::RGBA16_UINT;
            // 16-bit signed float
            case VK_FORMAT_R16_SFLOAT:
                return RHIFormat::R16_FLOAT;
            case VK_FORMAT_R16G16_SFLOAT:
                return RHIFormat::RG16_FLOAT;
            case VK_FORMAT_R16G16B16_SFLOAT:
                return RHIFormat::RGB16_FLOAT;
            case VK_FORMAT_R16G16B16A16_SFLOAT:
                return RHIFormat::RGBA16_FLOAT;

            // 32-bit
            case VK_FORMAT_R32_UINT:
                return RHIFormat::R32_UINT;
            case VK_FORMAT_R32G32_UINT:
                return RHIFormat::RG32_UINT;
            case VK_FORMAT_R32G32B32_UINT:
                return RHIFormat::RGB32_UINT;
            case VK_FORMAT_R32G32B32A32_UINT:
                return RHIFormat::RGBA32_UINT;

            case VK_FORMAT_R32_SINT:
                return RHIFormat::R32_SINT;
            case VK_FORMAT_R32G32_SINT:
                return RHIFormat::RG32_SINT;
            case VK_FORMAT_R32G32B32_SINT:
                return RHIFormat::RGB32_SINT;
            case VK_FORMAT_R32G32B32A32_SINT:
                return RHIFormat::RGBA32_SINT;

            case VK_FORMAT_R32_SFLOAT:
                return RHIFormat::R32_FLOAT;
            case VK_FORMAT_R32G32_SFLOAT:
                return RHIFormat::RG32_FLOAT;
            case VK_FORMAT_R32G32B32_SFLOAT:
                return RHIFormat::RGB32_FLOAT;
            case VK_FORMAT_R32G32B32A32_SFLOAT:
                return RHIFormat::RGBA32_FLOAT;

            // Packed
            case VK_FORMAT_A2B10G10R10_UNORM_PACK32:
                return RHIFormat::RGB10A2;
            case VK_FORMAT_B10G11R11_UFLOAT_PACK32:
                return RHIFormat::R11G11B10_FLOAT;
            case VK_FORMAT_E5B9G9R9_UFLOAT_PACK32:
                return RHIFormat::RGB9E5;

            // Depth / Stencil
            case VK_FORMAT_D16_UNORM:
                return RHIFormat::D16;
            case VK_FORMAT_X8_D24_UNORM_PACK32:
                return RHIFormat::D24;
            case VK_FORMAT_D32_SFLOAT:
                return RHIFormat::D32;
            case VK_FORMAT_D24_UNORM_S8_UINT:
                return RHIFormat::D24S8;
            case VK_FORMAT_D32_SFLOAT_S8_UINT:
                return RHIFormat::D32S8;

            // Compressed
            case VK_FORMAT_BC1_RGBA_UNORM_BLOCK:
                return RHIFormat::BC1;
            case VK_FORMAT_BC3_UNORM_BLOCK:
                return RHIFormat::BC3;
            case VK_FORMAT_BC5_UNORM_BLOCK:
                return RHIFormat::BC5;
            case VK_FORMAT_BC7_UNORM_BLOCK:
                return RHIFormat::BC7;

            case VK_FORMAT_ETC2_R8G8B8_UNORM_BLOCK:
                return RHIFormat::ETC2_RGB8;
            case VK_FORMAT_ETC2_R8G8B8A8_UNORM_BLOCK:
                return RHIFormat::ETC2_RGBA8;

            case VK_FORMAT_ASTC_4x4_UNORM_BLOCK:
                return RHIFormat::ASTC_4x4;
            case VK_FORMAT_ASTC_8x8_UNORM_BLOCK:
                return RHIFormat::ASTC_8x8;

            default:
                return RHIFormat::Undefined;
        }
    }

    VkFormat VulkanFormatGet(RHIFormat format)
    {
        switch (format)
        {
            case RHIFormat::R8:
                return VK_FORMAT_R8_UNORM;
            case RHIFormat::RG8:
                return VK_FORMAT_R8G8_UNORM;
            case RHIFormat::RGB8:
                return VK_FORMAT_R8G8B8_UNORM;
            case RHIFormat::RGBA8:
                return VK_FORMAT_R8G8B8A8_UNORM;
            case RHIFormat::BGRA8:
                return VK_FORMAT_B8G8R8A8_UNORM;

            case RHIFormat::R8sRGB:
                return VK_FORMAT_R8_SRGB;
            case RHIFormat::RG8sRGB:
                return VK_FORMAT_R8G8_SRGB;
            case RHIFormat::RGB8sRGB:
                return VK_FORMAT_R8G8B8_SRGB;
            case RHIFormat::RGBA8sRGB:
                return VK_FORMAT_R8G8B8A8_SRGB;
            case RHIFormat::BGRA8sRGB:
                return VK_FORMAT_B8G8R8A8_SRGB;

            case RHIFormat::R16_UINT:
                return VK_FORMAT_R16_UNORM;
            case RHIFormat::RG16_UINT:
                return VK_FORMAT_R16G16_UNORM;
            case RHIFormat::RGB16_UINT:
                return VK_FORMAT_R16G16B16_UNORM;
            case RHIFormat::RGBA16_UINT:
                return VK_FORMAT_R16G16B16A16_UNORM;

            case RHIFormat::R16_FLOAT:
                return VK_FORMAT_R16_SFLOAT;
            case RHIFormat::RG16_FLOAT:
                return VK_FORMAT_R16G16_SFLOAT;
            case RHIFormat::RGB16_FLOAT:
                return VK_FORMAT_R16G16B16_SFLOAT;
            case RHIFormat::RGBA16_FLOAT:
                return VK_FORMAT_R16G16B16A16_SFLOAT;

            case RHIFormat::R32_UINT:
                return VK_FORMAT_R32_UINT;
            case RHIFormat::RG32_UINT:
                return VK_FORMAT_R32G32_UINT;
            case RHIFormat::RGB32_UINT:
                return VK_FORMAT_R32G32B32_UINT;
            case RHIFormat::RGBA32_UINT:
                return VK_FORMAT_R32G32B32A32_UINT;

            case RHIFormat::R32_SINT:
                return VK_FORMAT_R32_SINT;
            case RHIFormat::RG32_SINT:
                return VK_FORMAT_R32G32_SINT;
            case RHIFormat::RGB32_SINT:
                return VK_FORMAT_R32G32B32_SINT;
            case RHIFormat::RGBA32_SINT:
                return VK_FORMAT_R32G32B32A32_SINT;

            case RHIFormat::R32_FLOAT:
                return VK_FORMAT_R32_SFLOAT;
            case RHIFormat::RG32_FLOAT:
                return VK_FORMAT_R32G32_SFLOAT;
            case RHIFormat::RGB32_FLOAT:
                return VK_FORMAT_R32G32B32_SFLOAT;
            case RHIFormat::RGBA32_FLOAT:
                return VK_FORMAT_R32G32B32A32_SFLOAT;

            case RHIFormat::RGB10A2:
                return VK_FORMAT_A2B10G10R10_UNORM_PACK32;
            case RHIFormat::R11G11B10_FLOAT:
                return VK_FORMAT_B10G11R11_UFLOAT_PACK32;
            case RHIFormat::RGB9E5:
                return VK_FORMAT_E5B9G9R9_UFLOAT_PACK32;

            case RHIFormat::D16:
                return VK_FORMAT_D16_UNORM;
            case RHIFormat::D24:
                return VK_FORMAT_X8_D24_UNORM_PACK32;
            case RHIFormat::D32:
                return VK_FORMAT_D32_SFLOAT;
            case RHIFormat::D24S8:
                return VK_FORMAT_D24_UNORM_S8_UINT;
            case RHIFormat::D32S8:
                return VK_FORMAT_D32_SFLOAT_S8_UINT;

            case RHIFormat::BC1:
                return VK_FORMAT_BC1_RGBA_UNORM_BLOCK;
            case RHIFormat::BC3:
                return VK_FORMAT_BC3_UNORM_BLOCK;
            case RHIFormat::BC5:
                return VK_FORMAT_BC5_UNORM_BLOCK;
            case RHIFormat::BC7:
                return VK_FORMAT_BC7_UNORM_BLOCK;

            case RHIFormat::ETC2_RGB8:
                return VK_FORMAT_ETC2_R8G8B8_UNORM_BLOCK;
            case RHIFormat::ETC2_RGBA8:
                return VK_FORMAT_ETC2_R8G8B8A8_UNORM_BLOCK;

            case RHIFormat::ASTC_4x4:
                return VK_FORMAT_ASTC_4x4_UNORM_BLOCK;
            case RHIFormat::ASTC_8x8:
                return VK_FORMAT_ASTC_8x8_UNORM_BLOCK;

            case RHIFormat::Undefined:
            default:
                return VK_FORMAT_UNDEFINED;
        }
    }

    RHISharingMode RHIAccessModeGet(VkSharingMode mode)
    {
        switch (mode)
        {
            case VK_SHARING_MODE_EXCLUSIVE:
                return RHISharingMode::Private;
            case VK_SHARING_MODE_CONCURRENT:
                return RHISharingMode::Shared;
            default:
                return RHISharingMode::Private;
        }
    }

    VkSharingMode VulkanSharingModeGet(RHISharingMode mode)
    {
        switch (mode)
        {
            case RHISharingMode::Private:
                return VK_SHARING_MODE_EXCLUSIVE;
            case RHISharingMode::Shared:
                return VK_SHARING_MODE_CONCURRENT;
            default:
                return VK_SHARING_MODE_EXCLUSIVE;
        }
    }

}// namespace Wl