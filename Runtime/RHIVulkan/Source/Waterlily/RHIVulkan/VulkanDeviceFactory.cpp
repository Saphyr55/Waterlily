#include "Waterlily/RHIVulkan/VulkanDeviceFactory.hpp"
#include "Waterlily/Core/Memory/MemoryScope.hpp"
#include "Waterlily/RHIVulkan/VulkanDevice.hpp"
#include "Waterlily/Core/Memory/Memory.hpp"

extern "C" Wl::RHIDevice* RHIDeviceCreateImpl()
{
    Wl::Allocator* allocator = Wl::MemoryStack::GetCurrentAllocator();
    return Wl::New(allocator, Wl::VulkanDevice(allocator));
}
