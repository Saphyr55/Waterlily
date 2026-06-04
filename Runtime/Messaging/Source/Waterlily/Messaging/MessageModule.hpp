#pragma once

#include "MessageExports.hpp"
#include "Waterlily/Core/Modules/Module.hpp"

namespace Wl
{

    class WL_MESSAGING_API MessagingModule : public Module
    {
    public:
        virtual void OnLoad() override;

        virtual void OnStartup() override;

        virtual void OnShutdown() override;

        virtual void OnUnload() override;

        virtual ~MessagingModule() = default;
    };

}// namespace Wl