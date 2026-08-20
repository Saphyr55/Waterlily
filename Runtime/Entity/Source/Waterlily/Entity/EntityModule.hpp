#pragma once

#include "Waterlily/Core/Modules/Module.hpp"
#include "Waterlily/Entity/EntityExports.hpp"

namespace Wl
{

    class WL_ENTITY_API EntityModule : public Module
    {
    public:
        virtual void OnStartup() override;

        virtual void OnShutdown() override;
    };

}// namespace Wl
