#pragma once

#include "Waterlily/Core/Modules/Module.hpp"

namespace Wl
{

    class SceneModule : public Module
    {
    public:
        virtual void OnLoad() override;

        virtual void OnStartup() override;

        virtual void OnShutdown() override;

        virtual void OnUnload() override;
    };

}// namespace Wl
