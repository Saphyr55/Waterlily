#pragma once

#include "Waterlily/Core/Containers/Array.hpp"
#include "Waterlily/Core/String/StringRef.hpp"
#include "Waterlily/RHIVulkan/VulkanRenderSurface.hpp"

#include <vulkan/vulkan_core.h>

namespace Wl
{

    class VulkanInstance;

    struct VulkanPhysicalDeviceInformation
    {
        VkPhysicalDeviceMemoryProperties MemoryProperties;
        VkPhysicalDeviceProperties Properties;
        VkPhysicalDeviceFeatures Features;
        VkPhysicalDeviceFeatures2 Features2;
        VkPhysicalDeviceVulkan12Features VulkanFeatures12;
        VkPhysicalDeviceDescriptorIndexingFeatures DescriptorIndexingFeatures;
        Array<VkQueueFamilyProperties> QueueFamilies;

        uint32_t GraphicsQueueIndex = UINT32_MAX;
        uint32_t PresentQueueIndex = UINT32_MAX;

        bool IsSuitable;
    };
    
    struct VulkanPhysicalDeviceRequirements
    {
        VkPhysicalDeviceType PreferredDeviceType = VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU;
        Array<StringRef> RequiredExtensions;
    };

    class VulkanPhysicalDeviceSelector
    {
    public:
        bool SelectPhysicalDevice();

        void QueryInfo();

        VkPhysicalDeviceProperties QueryProperties();

        VkPhysicalDeviceFeatures QueryFeatures();

        Array<VkQueueFamilyProperties> QueryQueueFamilies();

        bool SelectQueueFamilies();

        bool IsValidQueueFamily(const VkQueueFamilyProperties& queueFamilyProperties, int32_t queueFamilyIndex);

        bool IsPropertiesSuitable() const;

        bool IsFeaturesSuitable() const;

        bool CheckExtensionSupport();

    public:
        inline const VulkanPhysicalDeviceInformation& GetInfo()
        {
            return m_info;
        }

    public:
        VulkanPhysicalDeviceSelector(const VulkanPhysicalDeviceRequirements& requirements);
        ~VulkanPhysicalDeviceSelector() = default;

    private:
        VulkanPhysicalDeviceRequirements m_requirements;
        VulkanPhysicalDeviceInformation m_info;
        VulkanContext& m_context;
    };

}// namespace Wl
