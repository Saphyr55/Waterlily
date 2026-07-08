#pragma once

#include "Waterlily/Core/Containers/HashMap.hpp"
#include "Waterlily/Core/String/StringID.hpp"
#include "Waterlily/Renderer/Proxy/RenderProxy.hpp"

namespace Wl
{

    class CompositeRenderProxy : public RenderProxy
    {
    public:
        void AddProxy(StringID name, RenderProxy* proxy);
        void RemoveProxy(StringID name);

        virtual void PrepareFrame(FramePacket& packet, Frame& frame) override;

    private:
        HashMap<StringID, RenderProxy*> m_proxies;
    };

}// namespace Wl