#pragma once

#include "MessageExports.hpp"
#include "Waterlily/Core/Memory/SharedPtr.hpp"

namespace Wl
{

    class IMessageContext;

    class WL_MESSAGING_API IMessageReceiver
    {
    public:
        virtual void ReceiveMessage(const SharedPtr<IMessageContext>& context) = 0;

        virtual ~IMessageReceiver() = default;
    };

}// namespace Wl