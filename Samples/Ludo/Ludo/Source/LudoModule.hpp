#pragma once

#include "LudoExports.hpp"
#include "Waterlily/Core/Modules/Module.hpp"

namespace Wl
{

    class LUDO_API LudoModule : public Module
    {
    public:
        virtual void OnStartup() override;

        virtual void OnShutdown() override;
    };

}// namespace Wl
