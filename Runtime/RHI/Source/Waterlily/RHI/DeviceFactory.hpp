#pragma once

#include "Waterlily/Core/Memory/SharedPtr.hpp"
#include "Waterlily/Core/String/StringRef.hpp"
#include "Waterlily/RHI/RHIExports.hpp"


namespace Wl
{
    enum class RHIGraphicsAPI
    {
        Vulkan,
    };

    WL_RHI_API StringRef RHIModuleGetBackendName(RHIGraphicsAPI graphicsAPI);

    class RHIDevice;

    using RHICreateDeviceFn = RHIDevice* (*)();

    class WL_RHI_API RHIDeviceFactory
    {
    public:
        static SharedPtr<RHIDevice> Create(RHIGraphicsAPI);
    };

}// namespace Wl