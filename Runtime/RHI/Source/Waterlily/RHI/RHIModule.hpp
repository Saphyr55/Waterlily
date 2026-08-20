#pragma once

#include "Waterlily/Core/Modules/Module.hpp"
#include "Waterlily/RHI/RHIExports.hpp"

namespace Wl
{

    class WL_RHI_API RHIModule : public Module
    {
    public:
        virtual void OnStartup() override;

        virtual void OnShutdown() override;
    };

}// namespace Wl