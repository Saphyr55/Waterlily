#include "MessageImpl.hpp"

namespace Wl
{

    SharedPtr<IMessage> MessageContext::GetMessageInterface() const
    {
        return m_message;
    }

    const Array<MessageAddress>& MessageContext::GetReceipents() const
    {
        return m_receipents;
    }

    MessageAddress MessageContext::GetSender() const
    {
        return m_sender;
    }

}// namespace Wl