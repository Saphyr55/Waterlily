#include "Waterlily/RHI/DeviceFactory.hpp"
#include "Waterlily/Core/Defines.hpp"
#include "Waterlily/Core/Memory/SharedPtr.hpp"
#include "Waterlily/Core/Modules/ModuleRegistry.hpp"
#include "Waterlily/Core/Platform/DynamicLibrary.hpp"
#include "Waterlily/Core/String/StringRef.hpp"
#include "Waterlily/RHI/Device.hpp"

namespace Wl
{

    StringRef RHIModuleGetBackendName(RHIGraphicsAPI graphicsAPI)
    {
        StringRef fallback = "Waterlily.RHI.Vulkan";
        switch (graphicsAPI)
        {
            case RHIGraphicsAPI::Vulkan:
                return "Waterlily.RHI.Vulkan";
            default:
                return fallback;
        }
        return fallback;
    }

    SharedPtr<RHIDevice> RHIDeviceFactory::Create(RHIGraphicsAPI graphicsAPI)
    {
        ModuleRegistry& moduleRegistry = ModuleRegistry::GetInstance();
        StringRef moduleName = RHIModuleGetBackendName(graphicsAPI);

        ModuleRegistry::LoadedModule* loadedModule = moduleRegistry.GetLoadedModule(moduleName);
        WL_CHECK_MSG(loadedModule, "Failed to get RHI backend.");

        RHICreateDeviceFn RHIDeviceCreateImpl = DynamicLibraryLoader::LoadFunction<RHICreateDeviceFn>(loadedModule->Library, "RHIDeviceCreateImpl");
        WL_CHECK_MSG(RHIDeviceCreateImpl, "The RHI Backend do not provide a RHIDeviceCreateImpl factory function.");

        RHIDevice* device = RHIDeviceCreateImpl();

        return SharedPtr<RHIDevice>(device);
    }

}// namespace Wl
