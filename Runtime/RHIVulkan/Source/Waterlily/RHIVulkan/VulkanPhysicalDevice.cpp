
#include "Waterlily/Core/Defines.hpp"
#include "Waterlily/Core/Trace/Trace.hpp"

#include "Waterlily/RHIVulkan/VulkanContext.hpp"
#include "Waterlily/RHIVulkan/VulkanLoader.hpp"
#include "Waterlily/RHIVulkan/VulkanRenderSurface.hpp"

#include <vulkan/vulkan_core.h>

namespace Wl
{

    VulkanPhysicalDeviceSelector::VulkanPhysicalDeviceSelector(VulkanContext& context, const Array<StringRef>& extensions)
        : m_context(context)
        , m_extensions(extensions)
    {
    }

    VkPhysicalDeviceProperties VulkanPhysicalDeviceSelector::QueryProperties()
    {
        VulkanAPI::vkGetPhysicalDeviceProperties(m_context.PhysicalDevice, &m_info.Properties);
        return m_info.Properties;
    }

    VkPhysicalDeviceFeatures VulkanPhysicalDeviceSelector::QueryFeatures()
    {
        VulkanAPI::vkGetPhysicalDeviceFeatures(m_context.PhysicalDevice, &m_info.Features);
        return m_info.Features;
    }

    bool VulkanPhysicalDeviceSelector::IsPropertiesSuitable() const
    {
        return (m_info.Properties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU &&
                m_info.Properties.limits.maxImageDimension2D >= 4096);
    }

    bool VulkanPhysicalDeviceSelector::IsFeaturesSuitable() const
    {
        return m_info.Features.geometryShader && m_info.Features.samplerAnisotropy;
    }

    bool VulkanPhysicalDeviceSelector::CheckExtensionSupport(const Array<StringRef>& extensions)
    {
        uint32_t count;
        WL_VULKAN_CHECK(
                VulkanAPI::vkEnumerateDeviceExtensionProperties(m_context.PhysicalDevice, nullptr, &count, nullptr));

        Array<VkExtensionProperties> availableExtensions;
        availableExtensions.Resize(count);
        WL_VULKAN_CHECK(VulkanAPI::vkEnumerateDeviceExtensionProperties(m_context.PhysicalDevice,
                                                                        nullptr,
                                                                        &count,
                                                                        availableExtensions.data()));

        Array<StringRef> copyExtensions = extensions;

        for (const VkExtensionProperties& extension: availableExtensions)
        {
            copyExtensions.Remove(extension.extensionName);
        }

        return copyExtensions.IsEmpty();
    }

    Array<VkQueueFamilyProperties> VulkanPhysicalDeviceSelector::QueryQueueFamilies()
    {
        uint32_t queueFamilyCount = 0;
        VulkanAPI::vkGetPhysicalDeviceQueueFamilyProperties(m_context.PhysicalDevice, &queueFamilyCount, nullptr);
        if (queueFamilyCount == 0)
        {
            return false;
        }

        m_info.QueueFamilies.Resize(queueFamilyCount);
        VulkanAPI::vkGetPhysicalDeviceQueueFamilyProperties(m_context.PhysicalDevice,
                                                            &queueFamilyCount,
                                                            m_info.QueueFamilies.data());

        return m_info.QueueFamilies;
    }

    bool VulkanPhysicalDeviceSelector::IsValidQueueFamily(const VkQueueFamilyProperties& queue_family_properties,
                                                          int32_t queue_family_index)
    {
        VkBool32 isPresentSupported = false;
        WL_VULKAN_CHECK(VulkanAPI::vkGetPhysicalDeviceSurfaceSupportKHR(m_context.PhysicalDevice,
                                                                        queue_family_index,
                                                                        m_context.Surface->GetHandle(),
                                                                        &isPresentSupported));
        return (queue_family_properties.queueFlags & VK_QUEUE_GRAPHICS_BIT) && isPresentSupported == VK_TRUE;
    }

    bool VulkanPhysicalDeviceSelector::SelectQueueFamilies()
    {
        for (uint32_t queueFamilyIndex = 0; queueFamilyIndex < m_info.QueueFamilies.size(); queueFamilyIndex++)
        {
            const VkQueueFamilyProperties& props = m_info.QueueFamilies[queueFamilyIndex];

            if (IsValidQueueFamily(props, queueFamilyIndex))
            {
                m_info.GraphicsQueueIndex = queueFamilyIndex;
                m_info.PresentQueueIndex = queueFamilyIndex;
                return true;
            }
        }

        return false;
    }

    void VulkanPhysicalDeviceSelector::QueryInfo()
    {
        QueryProperties();
        bool isSuitableDeviceProperties = IsPropertiesSuitable();

        QueryFeatures();
        bool isSuitableDeviceFeatures = IsFeaturesSuitable();

        QueryQueueFamilies();
        bool isSuitableQueueFamilies = SelectQueueFamilies();

        bool isSuitableExtensionSupport = CheckExtensionSupport(s_PhysicalDeviceExtensions);

        VulkanAPI::vkGetPhysicalDeviceMemoryProperties(m_context.PhysicalDevice, &m_info.MemoryProperties);

        VkPhysicalDeviceVulkan12Features supportedFeatures12 = {};
        supportedFeatures12.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_2_FEATURES;

        VkPhysicalDeviceDescriptorIndexingFeatures descriptorIndexingFeatures = {};
        descriptorIndexingFeatures.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DESCRIPTOR_INDEXING_FEATURES;
        descriptorIndexingFeatures.pNext = &supportedFeatures12;

        VkPhysicalDeviceFeatures2 features2 = {};
        features2.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2;
        features2.pNext = &descriptorIndexingFeatures;

        VulkanAPI::vkGetPhysicalDeviceFeatures2(m_context.PhysicalDevice, &features2);

        m_info.Features2 = features2;
        m_info.DescriptorIndexingFeatures = descriptorIndexingFeatures;
        m_info.VulkanFeatures12 = supportedFeatures12;

        // TODO: Check supported features 1.2 about updateAndFreeBit with storage buffer and uniform buffer.
        WL_CHECK_MSG(supportedFeatures12.runtimeDescriptorArray, "GPU does not support runtimeDescriptorArray.");
        WL_CHECK_MSG(descriptorIndexingFeatures.descriptorBindingPartiallyBound,
                   "GPU does not support descriptorBindingPartiallyBound.");

        m_info.IsSuitable = isSuitableDeviceProperties && isSuitableDeviceFeatures && isSuitableQueueFamilies &&
                            isSuitableExtensionSupport && descriptorIndexingFeatures.descriptorBindingPartiallyBound &&
                            supportedFeatures12.runtimeDescriptorArray;
    }

    bool VulkanPhysicalDeviceSelector::SelectPhysicalDevice()
    {
        VkInstance instance = m_context.Instance;

        uint32_t deviceCount = 0;
        WL_VULKAN_CHECK(VulkanAPI::vkEnumeratePhysicalDevices(instance, &deviceCount, nullptr));
        if (deviceCount == 0)
        {
            WL_LOG_ERROR("[Vulkan]", "No physical devices found.");
            return false;
        }

        Array<VkPhysicalDevice> devices;
        devices.Resize(deviceCount);
        WL_VULKAN_CHECK(VulkanAPI::vkEnumeratePhysicalDevices(instance, &deviceCount, devices.data()));

        for (const VkPhysicalDevice& physical_device: devices)
        {
            m_context.PhysicalDevice = physical_device;

            QueryInfo();

            // Check if the device meets the requirements
            if (m_info.IsSuitable)
            {
                WL_LOG_INFO("[Vulkan]", "Physical device selected successfully.");
                WL_LOG_ERROR("[Vulkan]", "No suitable physical device found.");

                WL_LOG_INFO("[Vulkan]", "Selected Physical Device Properties:");
                WL_LOG_INFO("[Vulkan]", Wl::Format("Device Name: %s", m_info.Properties.deviceName));
                WL_LOG_INFO("[Vulkan]",
                          Wl::Format("API Version: %u.%u.%u",
                                  VK_VERSION_MAJOR(m_info.Properties.apiVersion),
                                  VK_VERSION_MINOR(m_info.Properties.apiVersion),
                                  VK_VERSION_PATCH(m_info.Properties.apiVersion)));
                WL_LOG_INFO("[Vulkan]", Wl::Format("Driver Version: %u", m_info.Properties.driverVersion));
                WL_LOG_INFO("[Vulkan]", Wl::Format("Vendor ID: %u", m_info.Properties.vendorID));
                WL_LOG_INFO("[Vulkan]", Wl::Format("Device ID: %u", m_info.Properties.deviceID));
                WL_LOG_INFO("[Vulkan]", Wl::Format("Device Type: %d", m_info.Properties.deviceType));
                return true;
            }
        }

        return false;
    }

}// namespace Wl