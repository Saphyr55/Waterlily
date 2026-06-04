#pragma once

#include "MessageExports.hpp"
#include "MessageReceiver.hpp"
#include "Waterlily/Core/Containers/Array.hpp"
#include "Waterlily/Core/Memory/Cast.hpp"
#include "Waterlily/Core/Memory/SharedPtr.hpp"
#include "Waterlily/Core/String/String.hpp"

namespace Wl
{

    using MessageAddress = String;

    struct WL_MESSAGING_API IMessage
    {
        virtual ~IMessage() = default;
    };

    class WL_MESSAGING_API IMessageContext
    {
    public:
        virtual SharedPtr<IMessage> GetMessageInterface() const = 0;

        template<typename MessageDerivedType>
        inline SharedPtr<MessageDerivedType> GetMessage() const
        {
            return StaticPtrCast<MessageDerivedType>(GetMessageInterface());
        }

        virtual const Array<MessageAddress>& GetReceipents() const = 0;

        virtual MessageAddress GetSender() const = 0;

        virtual ~IMessageContext() = default;
    };

}// namespace Wl
