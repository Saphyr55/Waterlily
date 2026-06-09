#include "Waterlily/RHIVulkan/VulkanDeviceFactory.hpp"
#include "Waterlily/RHIVulkan/VulkanDevice.hpp"
#include "Waterlily/RHIVulkan/VulkanContext.hpp"
#include "Waterlily/Core/Memory/DefaultAllocator.hpp"
#include "Waterlily/Core/Memory/Memory.hpp"

extern "C" Wl::RHIDevice* RHIDeviceCreateImpl()
{
    return Wl::New(Wl::DefaultAllocator::GetInstance(), Wl::VulkanDevice());
}
