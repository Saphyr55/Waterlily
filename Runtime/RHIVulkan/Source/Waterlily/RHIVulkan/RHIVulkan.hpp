#pragma once

#include "Waterlily/Core/Containers/Array.hpp"

#include <vulkan/vulkan_core.h>

namespace Wl
{

    class VulkanRHI
    {
    public:
        static Array<VkExtensionProperties> GetInstanceExtensionProperties();
    };

}// namespace Wl