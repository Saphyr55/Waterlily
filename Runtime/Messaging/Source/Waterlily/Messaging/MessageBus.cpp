#include "Waterlily/Messaging/MessageBus.hpp"
#include "Waterlily/Core/Memory/SharedPtr.hpp"

#include "MessageImpl.hpp"

namespace Wl
{

    MessageBus::MessageBus()
        : m_receivers(64)
        , m_pendingMessages(64)
    {
    }

    void MessageBus::RegisterReceiver(const MessageAddress& address, const SharedPtr<IMessageReceiver>& receiver)
    {
        if (!m_receivers.Contains(address))
        {
            m_receivers.Put(address, Array<SharedPtr<IMessageReceiver>>());
        }
        m_receivers.Get(address).Append(receiver);
    }

    void MessageBus::UnregisterReceiver(const MessageAddress& receiver)
    {
        m_receivers.Remove(receiver);
    }

    void MessageBus::SendMessage(const MessageAddress& address, const SharedPtr<IMessage>& message)
    {
        Array<MessageAddress> receipents;// TODO:
        m_pendingMessages.Append(MakeShared<MessageContext>(receipents, address, message));
    }

    void MessageBus::Dispatch(const MessageAddress& address)
    {
        Array<SharedPtr<IMessageContext>> toDispatch(64);

        for (auto it = m_pendingMessages.begin(); it != m_pendingMessages.end();)
        {
            if ((*it)->GetSender() == address)
            {
                toDispatch.push_back(*it);
                m_pendingMessages.Remove(*it);
            }
            else
            {
                ++it;
            }
        }

        if (auto receipents = m_receivers.GetPtr(address))
        {
            for (auto& context: toDispatch)
            {
                for (auto& receipent: *receipents)
                {
                    if (receipent)
                    {
                        receipent->ReceiveMessage(context);
                    }
                }
            }
        }
    }

    void MessageBus::ProcessMessages()
    {
        while (!m_pendingMessages.IsEmpty())
        {
            SharedPtr<IMessageContext> context = m_pendingMessages.Back();
            m_pendingMessages.Pop();
            if (auto* it = m_receivers.GetPtr(context->GetSender()))
            {
                for (SharedPtr<IMessageReceiver>& receiver: *it)
                {
                    if (receiver)
                    {
                        receiver->ReceiveMessage(context);
                    }
                }
            }
        }
    }

}// namespace Wl