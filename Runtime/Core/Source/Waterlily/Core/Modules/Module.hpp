#pragma once

#include "Waterlily/Core/CoreExports.hpp"

namespace Wl
{

    class WL_CORE_API Module
    {
    public:
        virtual void OnLoad() = 0;

        virtual void OnStartup() = 0;

        virtual void OnShutdown() = 0;

        virtual void OnUnload() = 0;

        virtual ~Module() = default;
    };

}// namespace Wl
