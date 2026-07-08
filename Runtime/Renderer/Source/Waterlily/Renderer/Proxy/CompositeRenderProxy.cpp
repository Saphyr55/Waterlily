#include "Waterlily/Renderer/Proxy/CompositeRenderProxy.hpp"

namespace Wl
{
    
    void CompositeRenderProxy::PrepareFrame(FramePacket& packet, Frame& frame)
    {
        for (auto [_, proxy]: m_proxies)
        {
            proxy->PrepareFrame(packet, frame);
        }
    }

}// namespace Wl