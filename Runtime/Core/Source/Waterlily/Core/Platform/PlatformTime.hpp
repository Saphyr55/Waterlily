#pragma once

#include "Waterlily/Core/CoreExports.hpp"
#include "Waterlily/Core/Defines.hpp"

namespace Wl
{

    WL_CORE_API void PlatformTimeInit();

    WL_CORE_API void PlatformDelay(double ms);

    WL_CORE_API double PlatformGetHighResolutionTime();

}// namespace Wl