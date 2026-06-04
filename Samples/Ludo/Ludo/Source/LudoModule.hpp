#pragma once

#include "LudoExports.hpp"

#include "Waterlily/Core/Modules/Module.hpp"

using namespace Wl;

class LUDO_API LudoModule : public Module
{
public:
    virtual void OnLoad() override;

    virtual void OnStartup() override;

    virtual void OnShutdown() override;

    virtual void OnUnload() override;
};
