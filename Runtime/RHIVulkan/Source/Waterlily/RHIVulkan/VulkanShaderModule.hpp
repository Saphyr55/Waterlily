#pragma once

#include "Waterlily/Core/Containers/Array.hpp"
#include "Waterlily/Core/Defines.hpp"

#include <vulkan/vulkan_core.h>

namespace Wl
{

    struct VulkanContext;

    class VulkanShaderModule
    {
    public:
        void SetByteCode(const Array<uint8_t>& byteCode)
        {
            m_byteCode = byteCode;
        }

        void Create();

        void Destroy();

        VkShaderModule GetHandle();

    public:
        VulkanShaderModule() = default;
        VulkanShaderModule(const Array<uint8_t>& byteCode);

    private:
        VkShaderModule handle_ = VK_NULL_HANDLE;
        Array<uint8_t> m_byteCode;
    };

}// namespace Wl