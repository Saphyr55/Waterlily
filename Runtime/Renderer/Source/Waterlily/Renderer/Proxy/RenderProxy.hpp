#pragma once

namespace Wl
{

    class Frame;
    class FramePacket;

    class RenderProxy
    {
    public:
        virtual void PrepareFrame(FramePacket& packet, Frame& frame) = 0;
    };

}// namespace Wl
