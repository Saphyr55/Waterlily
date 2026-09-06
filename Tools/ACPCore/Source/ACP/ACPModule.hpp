#pragma once

#include "ACP/ACPExports.hpp"
#include "Waterlily/Core/Modules/Module.hpp"

namespace Wl
{

    class WL_TOOLS_ACP_API ACPModule : public Module
    {
    public:
        virtual void OnStartup() override;

        virtual void OnShutdown() override;
    };

}// namespace Wl
