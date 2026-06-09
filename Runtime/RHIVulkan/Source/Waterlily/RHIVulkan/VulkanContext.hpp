#pragma once

#include "Waterlily/Core/Function/FunctionRef.hpp"
#include "Waterlily/Core/Logging/Trace.hpp"
#include "Waterlily/Core/Memory/SharedPtr.hpp"
#include "Waterlily/Core/Platform/DynamicLibrary.hpp"
#include "Waterlily/RHI/RHIForwards.hpp"
#include "Waterlily/RHI/Types.hpp"
#include "Waterlily/RHIVulkan/VulkanLoader.hpp"
#include "Waterlily/RHIVulkan/VulkanPhysicalDevice.hpp"
#include "Waterlily/RHIVulkan/VulkanRenderSurface.hpp"

#include <vk_mem_alloc.h>

#define WL_VULKAN_CHECK(Expr)                                                                                   \
    do                                                                                                          \
    {                                                                                                           \
        VkResult __WL_vulkan_result__ = (Expr);                                                                 \
        WL_LOG_FATAL_WHEN(__WL_vulkan_result__ != VK_SUCCESS,                                                   \
                          "Vulkan", "Failed with the result '%s'", VulkanStringOfResult(__WL_vulkan_result__)); \
    } while (false)

namespace Wl
{

    static const Array<StringRef> s_PhysicalDeviceExtensions = {VK_KHR_SWAPCHAIN_EXTENSION_NAME,
                                                                VK_EXT_DESCRIPTOR_INDEXING_EXTENSION_NAME};

    struct VulkanContext
    {
        const uint32_t VulkanAPIVersion = VK_API_VERSION_1_4;
        SharedPtr<DynamicLibrary> Library;

        VkInstance Instance = VK_NULL_HANDLE;
        VkDevice Device = VK_NULL_HANDLE;
        VkPhysicalDevice PhysicalDevice = VK_NULL_HANDLE;
        VulkanPhysicalDeviceInformation PhysicalDeviceInfo;

        Array<SharedPtr<RHICommandQueue>> CommandQueues;

        SharedPtr<VulkanRenderSurface> Surface;

        VkDebugUtilsMessengerEXT DebugUtilsMessenger = VK_NULL_HANDLE;
        VkAllocationCallbacks* Allocator = nullptr;
        VmaAllocator VmaAllocator;
    };

    VulkanContext& VulkanContextGet();

    // Vulkan Context functions.
    void VulkanContextCreate(VulkanContext& context, void* windowHandle);
    void VulkanContextDestroy(VulkanContext& context);

    // Debug messenger.
    void VulkanDebugMessengerCreate(VulkanContext& context);
    void VulkanDebugMessengerDestroy(VulkanContext& context);

    // Vulkan Instance functions.
    void VulkanInstanceCreate(VulkanContext& context);
    void VulkanInstanceDestroy(VulkanContext& context);

    // Device
    void VulkanDeviceCreate(VulkanContext& context, VulkanPhysicalDeviceSelector& physicalDeviceSelector);
    void VulkanDeviceDestroy(VulkanContext& context);

    // String conversion
    const char* VulkanStringOfPresentMode(VkPresentModeKHR presentMode);
    const char* VulkanStringOfResult(VkResult result);

    // Queues functions.
    RHIQueueType VulkanQueueType(VulkanContext& context, uint32_t queueFamilyIndex);
    Array<uint32_t> VulkanQueryQueueFamilyIndices(
            VulkanContext& context,
            FunctionRef<bool(const VkQueueFamilyProperties&, uint32_t)> predicate = [](auto&, uint32_t)
    {
        return true;
    });
    VkQueue VulkanQueryQueue(VulkanContext& context, uint32_t queueFamilyIndex, uint32_t queueIndex);
    bool VulkanQueuePresentModeIsSupported(VulkanContext& context, uint32_t queueFamilyIndex);

    // Converts RHIAccessMode to VkSharingMode and vice versa.
    RHISharingMode RHIAccessModeGet(VkSharingMode mode);
    VkSharingMode VulkanSharingModeGet(RHISharingMode mode);

    // Converts RHIFormat to VkFormat and vice versa.
    VkFormat VulkanFormatGet(RHIFormat format);
    RHIFormat RHIFormatGet(VkFormat format);

    inline VkMemoryPropertyFlags VulkanMemoryUsageGet(RHIMemoryUsage memory_usage)
    {
        VkMemoryPropertyFlags properties = 0;
        if (memory_usage == RHIMemoryUsage::Device)
        {
            properties |= VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;
        }
        else if (memory_usage == RHIMemoryUsage::Host)
        {
            properties |= (VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
        }
        return properties;
    }

    inline VkVertexInputRate VulkanVertexInputRateGet(RHIVertexInputRate rate)
    {
        switch (rate)
        {
            case RHIVertexInputRate::Vertex:
                return VK_VERTEX_INPUT_RATE_VERTEX;
            case RHIVertexInputRate::Instance:
                return VK_VERTEX_INPUT_RATE_INSTANCE;
            default:
                return VK_VERTEX_INPUT_RATE_VERTEX;
        }
    }

    inline RHIVertexInputRate RHIVertexInputRateGet(VkVertexInputRate rate)
    {
        switch (rate)
        {
            case VK_VERTEX_INPUT_RATE_VERTEX:
                return RHIVertexInputRate::Vertex;
            case VK_VERTEX_INPUT_RATE_INSTANCE:
                return RHIVertexInputRate::Instance;
            default:
                return RHIVertexInputRate::Vertex;
        }
    }

    inline bool RHIBufferUsageHas(RHIBufferUsageFlags value, RHIBufferUsageFlags flag)
    {
        return (static_cast<uint8_t>(value) & static_cast<uint8_t>(flag)) != 0;
    }

    inline bool RHITextureUsageHas(RHITextureUsageFlags value, RHITextureUsageFlags flag)
    {
        return (static_cast<uint8_t>(value) & static_cast<uint8_t>(flag)) != 0;
    }

    inline VkAttachmentLoadOp VulkanLoadOpGet(RHIAttachmentLoadOp op)
    {
        switch (op)
        {
            case RHIAttachmentLoadOp::Load:
                return VK_ATTACHMENT_LOAD_OP_LOAD;
            case RHIAttachmentLoadOp::Clear:
                return VK_ATTACHMENT_LOAD_OP_CLEAR;
            case RHIAttachmentLoadOp::DontCare:
                return VK_ATTACHMENT_LOAD_OP_DONT_CARE;
            case RHIAttachmentLoadOp::None:
            default:
                return VK_ATTACHMENT_LOAD_OP_DONT_CARE;
        }
    }

    inline VkAttachmentStoreOp VulkanStoreOpGet(RHIAttachmentStoreOp op)
    {
        switch (op)
        {
            case RHIAttachmentStoreOp::Store:
                return VK_ATTACHMENT_STORE_OP_STORE;
            case RHIAttachmentStoreOp::DontCare:
                return VK_ATTACHMENT_STORE_OP_DONT_CARE;
            case RHIAttachmentStoreOp::None:
            default:
                return VK_ATTACHMENT_STORE_OP_DONT_CARE;
        }
    }

    inline RHIAttachmentStoreOp RHIStoreOpGet(VkAttachmentStoreOp op)
    {
        switch (op)
        {
            case VK_ATTACHMENT_STORE_OP_STORE:
                return RHIAttachmentStoreOp::Store;
            case VK_ATTACHMENT_STORE_OP_DONT_CARE:
            default:
                return RHIAttachmentStoreOp::DontCare;
        }
    }

    inline RHIAttachmentLoadOp RHILoadOpGet(VkAttachmentLoadOp op)
    {
        switch (op)
        {
            case VK_ATTACHMENT_LOAD_OP_LOAD:
                return RHIAttachmentLoadOp::Load;
            case VK_ATTACHMENT_LOAD_OP_CLEAR:
                return RHIAttachmentLoadOp::Clear;
            case VK_ATTACHMENT_LOAD_OP_DONT_CARE:
            default:
                return RHIAttachmentLoadOp::DontCare;
        }
    }

    inline VkImageLayout VulkanTextureLayoutGet(RHITextureLayout layout)
    {
        switch (layout)
        {
            case RHITextureLayout::Undefined:
                return VK_IMAGE_LAYOUT_UNDEFINED;
            case RHITextureLayout::TransferDst:
                return VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
            case RHITextureLayout::TransferSrc:
                return VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
            case RHITextureLayout::ShaderReadOnly:
                return VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
            case RHITextureLayout::ColorAttachment:
                return VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
            case RHITextureLayout::DepthStencilAttachment:
                return VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
            case RHITextureLayout::Present:
                return VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
            case RHITextureLayout::General:
                return VK_IMAGE_LAYOUT_GENERAL;
            default:
                return VK_IMAGE_LAYOUT_UNDEFINED;
        }
    }

    inline RHITextureLayout RHITextureLayoutGet(VkImageLayout layout)
    {
        switch (layout)
        {
            case VK_IMAGE_LAYOUT_UNDEFINED:
                return RHITextureLayout::Undefined;
            case VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL:
                return RHITextureLayout::TransferDst;
            case VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL:
                return RHITextureLayout::TransferSrc;
            case VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL:
                return RHITextureLayout::ShaderReadOnly;
            case VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL:
                return RHITextureLayout::ColorAttachment;
            case VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL:
                return RHITextureLayout::DepthStencilAttachment;
            case VK_IMAGE_LAYOUT_PRESENT_SRC_KHR:
                return RHITextureLayout::Present;
            case VK_IMAGE_LAYOUT_GENERAL:
                return RHITextureLayout::General;
            default:
                return RHITextureLayout::Undefined;
        }
    }

    inline VkImageUsageFlags VulkanTextureUsageGet(RHITextureUsageFlags usage)
    {
        VkImageUsageFlags flags = 0;

        if (RHITextureUsageHas(usage, RHITextureUsageFlags::ColorAttachment))
        {
            flags |= VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
        }
        if (RHITextureUsageHas(usage, RHITextureUsageFlags::DepthStencilAttachment))
        {
            flags |= VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT;
        }
        if (RHITextureUsageHas(usage, RHITextureUsageFlags::Sampler))
        {
            flags |= VK_IMAGE_USAGE_SAMPLED_BIT;
        }
        if (RHITextureUsageHas(usage, RHITextureUsageFlags::TransferSrc))
        {
            flags |= VK_IMAGE_USAGE_TRANSFER_SRC_BIT;
        }
        if (RHITextureUsageHas(usage, RHITextureUsageFlags::TransferDst))
        {
            flags |= VK_IMAGE_USAGE_TRANSFER_DST_BIT;
        }

        // TODO:
        // - VK_IMAGE_USAGE_STORAGE_BIT
        // - VK_IMAGE_USAGE_INPUT_ATTACHMENT_BIT

        return flags;
    }

    inline RHITextureUsageFlags RHITextureUsageGet(VkImageUsageFlags usage)
    {
        RHITextureUsageFlags flags = static_cast<RHITextureUsageFlags>(0);

        if (usage & VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT)
        {
            flags |= RHITextureUsageFlags::ColorAttachment;
        }
        if (usage & VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT)
        {
            flags |= RHITextureUsageFlags::DepthStencilAttachment;
        }
        if (usage & VK_IMAGE_USAGE_SAMPLED_BIT)
        {
            flags |= RHITextureUsageFlags::Sampler;
        }
        if (usage & VK_IMAGE_USAGE_TRANSFER_SRC_BIT)
        {
            flags |= RHITextureUsageFlags::TransferSrc;
        }
        if (usage & VK_IMAGE_USAGE_TRANSFER_DST_BIT)
        {
            flags |= RHITextureUsageFlags::TransferDst;
        }

        return flags;
    }

    inline VkBufferUsageFlags VulkanBufferUsageGet(RHIBufferUsageFlags usage)
    {
        VkBufferUsageFlags flags = 0;

        if (RHIBufferUsageHas(usage, RHIBufferUsageFlags::Vertex))
        {
            flags |= VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
        }
        if (RHIBufferUsageHas(usage, RHIBufferUsageFlags::Index))
        {
            flags |= VK_BUFFER_USAGE_INDEX_BUFFER_BIT;
        }
        if (RHIBufferUsageHas(usage, RHIBufferUsageFlags::Uniform))
        {
            flags |= VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT;
        }
        if (RHIBufferUsageHas(usage, RHIBufferUsageFlags::Storage))
        {
            flags |= VK_BUFFER_USAGE_STORAGE_BUFFER_BIT;
        }
        if (RHIBufferUsageHas(usage, RHIBufferUsageFlags::TransferSrc))
        {
            flags |= VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
        }
        if (RHIBufferUsageHas(usage, RHIBufferUsageFlags::TransferDst))
        {
            flags |= VK_BUFFER_USAGE_TRANSFER_DST_BIT;
        }
        if (RHIBufferUsageHas(usage, RHIBufferUsageFlags::Indirect))
        {
            flags |= VK_BUFFER_USAGE_INDIRECT_BUFFER_BIT;
        }

        return flags;
    }

    inline RHIBufferUsageFlags RHIBufferUsageGet(VkBufferUsageFlags usage)
    {
        RHIBufferUsageFlags flags = RHIBufferUsageFlags::None;

        if (usage & VK_BUFFER_USAGE_VERTEX_BUFFER_BIT)
        {
            flags |= RHIBufferUsageFlags::Vertex;
        }
        if (usage & VK_BUFFER_USAGE_INDEX_BUFFER_BIT)
        {
            flags |= RHIBufferUsageFlags::Index;
        }
        if (usage & VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT)
        {
            flags |= RHIBufferUsageFlags::Uniform;
        }
        if (usage & VK_BUFFER_USAGE_STORAGE_BUFFER_BIT)
        {
            flags |= RHIBufferUsageFlags::Storage;
        }
        if (usage & VK_BUFFER_USAGE_TRANSFER_SRC_BIT)
        {
            flags |= RHIBufferUsageFlags::TransferSrc;
        }
        if (usage & VK_BUFFER_USAGE_TRANSFER_DST_BIT)
        {
            flags |= RHIBufferUsageFlags::TransferDst;
        }

        return flags;
    }

    inline uint32_t VulkanFindMemoryType(VulkanContext& context, uint32_t typeFilter, VkMemoryPropertyFlags properties)
    {
        VkPhysicalDeviceMemoryProperties memoryProperties;
        VulkanAPI::vkGetPhysicalDeviceMemoryProperties(context.PhysicalDevice, &memoryProperties);

        for (uint32_t i = 0; i < memoryProperties.memoryTypeCount; i++)
        {
            if (typeFilter & (1 << i) && (memoryProperties.memoryTypes[i].propertyFlags & properties) == properties)
            {
                return i;
            }
        }

        WL_CRASH("Failed to find suitable Vulkan memory type for requested properties.");
        return 0;
    }

    inline VkDescriptorType VulkanDescriptorTypeGet(RHIShaderResourceType type)
    {
        switch (type)
        {
            case RHIShaderResourceType::Uniform:
                return VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
            case RHIShaderResourceType::StorageBuffer:
                return VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
            case RHIShaderResourceType::StorageTexture:
                return VK_DESCRIPTOR_TYPE_STORAGE_IMAGE;
            case RHIShaderResourceType::Sampler:
                return VK_DESCRIPTOR_TYPE_SAMPLER;
            case RHIShaderResourceType::CombinedTextureSampler:
                return VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
            default:
                WL_LOG_FATAL("[Vulkan]", "Unknown RHIShaderResourceType.");
                return VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        }
    }

    inline VkShaderStageFlags VulkanShaderStageGet(RHIShaderStage stage)
    {
        VkShaderStageFlags flags = 0;
        if ((stage & RHIShaderStage::Vertex) == RHIShaderStage::Vertex)
        {
            flags |= VK_SHADER_STAGE_VERTEX_BIT;
        }

        if ((stage & RHIShaderStage::Fragment) == RHIShaderStage::Fragment)
        {
            flags |= VK_SHADER_STAGE_FRAGMENT_BIT;
        }

        if ((stage & RHIShaderStage::Tesselation) == RHIShaderStage::Tesselation)
        {
            flags |= VK_SHADER_STAGE_TESSELLATION_CONTROL_BIT;
            flags |= VK_SHADER_STAGE_TESSELLATION_EVALUATION_BIT;
        }

        if ((stage & RHIShaderStage::Geometry) == RHIShaderStage::Geometry)
        {
            flags |= VK_SHADER_STAGE_GEOMETRY_BIT;
        }

        if ((stage & RHIShaderStage::Compute) == RHIShaderStage::Compute)
        {
            flags |= VK_SHADER_STAGE_COMPUTE_BIT;
        }

        if (RHIShaderStage::AllGraphics == (stage & RHIShaderStage::AllGraphics))
        {
            flags |= VK_SHADER_STAGE_ALL_GRAPHICS;
        }

        return flags;
    }

    inline VkAccessFlags VulkanAccessGet(RHIBufferUsageFlags usage)
    {
        VkAccessFlags flags = 0;

        auto usageHas = [&](RHIBufferUsageFlags usage2) -> bool
        {
            return RHIBufferUsageHas(usage, usage2);
        };

        if (usageHas(RHIBufferUsageFlags::TransferSrc))
        {
            flags |= VK_ACCESS_TRANSFER_READ_BIT;
        }

        if (usageHas(RHIBufferUsageFlags::TransferDst))
        {
            flags |= VK_ACCESS_TRANSFER_WRITE_BIT;
        }

        if (usageHas(RHIBufferUsageFlags::Vertex))
        {
            flags |= VK_ACCESS_VERTEX_ATTRIBUTE_READ_BIT;
        }

        if (usageHas(RHIBufferUsageFlags::Index))
        {
            flags |= VK_ACCESS_INDEX_READ_BIT;
        }

        if (usageHas(RHIBufferUsageFlags::Uniform))
        {
            flags |= VK_ACCESS_UNIFORM_READ_BIT;
        }

        if (usageHas(RHIBufferUsageFlags::Storage))
        {
            flags |= VK_ACCESS_SHADER_READ_BIT | VK_ACCESS_SHADER_WRITE_BIT;
        }

        if (usageHas(RHIBufferUsageFlags::Indirect))
        {
            flags |= VK_ACCESS_INDIRECT_COMMAND_READ_BIT;
        }

        return flags;
    }

    inline VkPipelineStageFlags VulkanPipelineStageGet(RHIBufferUsageFlags usage)
    {
        VkPipelineStageFlags stages = 0;

        auto usageHas = [&](RHIBufferUsageFlags usage2) -> bool
        {
            return RHIBufferUsageHas(usage, usage2);
        };

        if (usageHas(RHIBufferUsageFlags::TransferSrc | RHIBufferUsageFlags::TransferDst))
            stages |= VK_PIPELINE_STAGE_TRANSFER_BIT;

        if (usageHas(RHIBufferUsageFlags::Vertex))
        {
            stages |= VK_PIPELINE_STAGE_VERTEX_INPUT_BIT;
        }

        if (usageHas(RHIBufferUsageFlags::Index))
        {
            stages |= VK_PIPELINE_STAGE_VERTEX_INPUT_BIT;
        }

        if (usageHas(RHIBufferUsageFlags::Uniform))
        {
            stages |= VK_PIPELINE_STAGE_VERTEX_SHADER_BIT | VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
        }

        if (usageHas(RHIBufferUsageFlags::Storage))
        {
            stages |= VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT | VK_PIPELINE_STAGE_VERTEX_SHADER_BIT |
                      VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
        }

        if (usageHas(RHIBufferUsageFlags::Indirect))
        {
            stages |= VK_PIPELINE_STAGE_DRAW_INDIRECT_BIT;
        }

        return stages;
    }

    inline VkAccessFlags VulkanAccessGet(RHITextureUsageFlags usage)
    {
        VkAccessFlags flags = 0;

        auto usageHas = [&](RHITextureUsageFlags usage2) -> bool
        {
            return RHITextureUsageHas(usage, usage2);
        };

        if (usageHas(RHITextureUsageFlags::TransferSrc))
        {
            flags |= VK_ACCESS_TRANSFER_READ_BIT;
        }

        if (usageHas(RHITextureUsageFlags::TransferDst))
        {
            flags |= VK_ACCESS_TRANSFER_WRITE_BIT;
        }

        if (usageHas(RHITextureUsageFlags::Sampler))
        {
            flags |= VK_ACCESS_SHADER_READ_BIT;
        }

        if (usageHas(RHITextureUsageFlags::Storage))
        {
            flags |= VK_ACCESS_SHADER_READ_BIT | VK_ACCESS_SHADER_WRITE_BIT;
        }

        if (usageHas(RHITextureUsageFlags::ColorAttachment))
        {
            flags |= VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
        }

        if (usageHas(RHITextureUsageFlags::DepthStencilAttachment))
        {
            flags |= VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
        }

        return flags;
    }

    inline VkPipelineStageFlags VulkanPipelineStageGet(RHITextureUsageFlags usage)
    {
        VkPipelineStageFlags stages = 0;

        auto usageHas = [&](RHITextureUsageFlags usage2) -> bool
        {
            return RHITextureUsageHas(usage, usage2);
        };

        if (usageHas((RHITextureUsageFlags::TransferSrc | RHITextureUsageFlags::TransferDst)))
        {
            stages |= VK_PIPELINE_STAGE_TRANSFER_BIT;
        }

        if (usageHas(RHITextureUsageFlags::Sampler))
        {
            stages |= VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT | VK_PIPELINE_STAGE_VERTEX_SHADER_BIT;
        }

        if (usageHas(RHITextureUsageFlags::Storage))
        {
            stages |= VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT | VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
        }

        if (usageHas(RHITextureUsageFlags::ColorAttachment))
        {
            stages |= VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
        }

        if (usageHas(RHITextureUsageFlags::DepthStencilAttachment))
        {
            stages |= VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT | VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT;
        }
        return stages;
    }

    inline VkFilter VulkanFilterGet(RHIFilter filter)
    {
        switch (filter)
        {
            case RHIFilter::NEAREST:
                return VK_FILTER_NEAREST;
            case RHIFilter::LINEAR:
                return VK_FILTER_LINEAR;
            default:
                return VK_FILTER_NEAREST;
        }
        return VK_FILTER_NEAREST;
    }

    inline VkSamplerAddressMode VulkanAddressModeGet(RHISamplerAddressMode mode)
    {
        switch (mode)
        {
            case RHISamplerAddressMode::Repeat:
                return VK_SAMPLER_ADDRESS_MODE_REPEAT;
            case RHISamplerAddressMode::MirroredRepeat:
                return VK_SAMPLER_ADDRESS_MODE_MIRRORED_REPEAT;
            case RHISamplerAddressMode::ClampToEdge:
                return VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
            case RHISamplerAddressMode::ClampToBorder:
                return VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_BORDER;
            default:
                return VK_SAMPLER_ADDRESS_MODE_REPEAT;
        }
        return VK_SAMPLER_ADDRESS_MODE_REPEAT;
    }

    inline VkImageAspectFlags VulkanImageAspectGet(RHIFormat format)
    {
        switch (format)
        {
            case RHIFormat::D16:
            case RHIFormat::D24:
            case RHIFormat::D32:
                return VK_IMAGE_ASPECT_DEPTH_BIT;
            case RHIFormat::D24S8:
            case RHIFormat::D32S8:
                return (VK_IMAGE_ASPECT_DEPTH_BIT | VK_IMAGE_ASPECT_STENCIL_BIT);
            case RHIFormat::Undefined:
                return VK_IMAGE_ASPECT_NONE;
            default:
                return VK_IMAGE_ASPECT_COLOR_BIT;
        }
    }

    inline VkQueueFlags VulkanQueueTypeGet(RHIQueueType type)
    {
        VkQueueFlags flags = 0;

        if ((type & RHIQueueType::Graphics) == RHIQueueType::Graphics)
        {
            flags |= VK_QUEUE_GRAPHICS_BIT;
        }
        if ((type & RHIQueueType::Compute) == RHIQueueType::Compute)
        {
            flags |= VK_QUEUE_COMPUTE_BIT;
        }
        if ((type & RHIQueueType::Transfer) == RHIQueueType::Transfer)
        {
            flags |= VK_QUEUE_TRANSFER_BIT;
        }

        return flags;
    }

}// namespace Wl