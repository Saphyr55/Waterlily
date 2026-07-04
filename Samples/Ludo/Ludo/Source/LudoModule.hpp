#pragma once

#include "LudoApplication.hpp"
#include "LudoExports.hpp"

#include "Waterlily/Core/Modules/Module.hpp"

namespace Wl
{

    class LUDO_API LudoModule : public Module
    {
    public:
        virtual void OnLoad() override;

        virtual void OnStartup() override;

        virtual void OnShutdown() override;

        virtual void OnUnload() override;

    private:
        LudoApplicationDelegate m_applicationState;
    };

}// namespace Wl
