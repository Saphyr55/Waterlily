#pragma once

#include "Waterlily/Assets/assetsExports.hpp"
#include "Waterlily/Core/Modules/Module.hpp"

namespace Wl
{

    class WL_ASSETS_API AssetsModule : public Module
    {
    public:
        virtual void OnLoad() override;

        virtual void OnStartup() override;

        virtual void OnShutdown() override;

        virtual void OnUnload() override;
    };

}// namespace Wl
