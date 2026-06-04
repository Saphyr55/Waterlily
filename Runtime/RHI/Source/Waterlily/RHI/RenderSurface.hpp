#pragma once

#include "Waterlily/Core/Defines.hpp"
#include "Waterlily/Core/Memory/SharedPtr.hpp"

namespace Wl
{

    class RHIRenderSurface
    {
    public:
        virtual void* GetPlatformWindow() = 0;
    };

    using RHIRenderSurfaceHandle = SharedPtr<RHIRenderSurface>;

}// namespace Wl