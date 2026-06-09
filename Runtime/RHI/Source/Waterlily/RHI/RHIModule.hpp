#pragma once

#include "Waterlily/Core/Modules/Module.hpp"
#include "Waterlily/RHI/Device.hpp"
#include "Waterlily/RHI/RHIExports.hpp"

namespace Wl
{

    class WL_RHI_API RHIModule : public Module
    {
    public:
        virtual void OnLoad() override;

        virtual void OnStartup() override;

        virtual void OnShutdown() override;

        virtual void OnUnload() override;
    };

}// namespace Wl