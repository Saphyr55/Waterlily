#pragma once

#include "Waterlily/Core/Memory/SharedPtr.hpp"
#include "Waterlily/Messaging/Message.hpp"

namespace Wl
{

    class MessageContext : public IMessageContext
    {
    public:
        virtual SharedPtr<IMessage> GetMessageInterface() const override;

        virtual const Array<MessageAddress>& GetReceipents() const override;

        virtual MessageAddress GetSender() const override;

        MessageContext(const Array<MessageAddress>& receipents,
                       const MessageAddress& sender,
                       const SharedPtr<IMessage>& message)
            : m_receipents(receipents)
            , m_sender(sender)
            , m_message(message)
        {
        }

    private:
        Array<MessageAddress> m_receipents;
        MessageAddress m_sender;
        SharedPtr<IMessage> m_message;
    };

}// namespace Wl