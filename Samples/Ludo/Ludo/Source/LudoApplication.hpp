#pragma once

#include "LudoExports.hpp"
#include "Waterlily/Core/Defines.hpp"
#include "Waterlily/Core/Memory/SharedPtr.hpp"

namespace Wl
{

    class RHIDevice;
    class FrameContext;
    class AssetManager;
    class Application;

    LUDO_API int32_t LudoApplication(Application& application);

}// namespace Wl
