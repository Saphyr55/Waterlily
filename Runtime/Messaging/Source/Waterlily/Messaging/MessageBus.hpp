#pragma once

#include "Waterlily/Core/Containers/HashMap.hpp"
#include "Waterlily/Core/Memory/SharedPtr.hpp"
#include "Waterlily/Messaging/Message.hpp"
#include "Waterlily/Messaging/MessageExports.hpp"
#include "Waterlily/Messaging/MessageReceiver.hpp"

namespace Wl
{

    class WL_MESSAGING_API MessageBus
    {
    public:
        void RegisterReceiver(const MessageAddress& address, const SharedPtr<IMessageReceiver>& receiver);

        void UnregisterReceiver(const MessageAddress& address);

        void SendMessage(const MessageAddress& address, const SharedPtr<IMessage>& message);

        void Dispatch(const MessageAddress& address);

        void ProcessMessages();

        MessageBus();
        ~MessageBus() = default;

    private:
        HashMap<MessageAddress, Array<SharedPtr<IMessageReceiver>>> m_receivers;
        Array<SharedPtr<IMessageContext>> m_pendingMessages;
    };

}// namespace Wl
