#include "Waterlily/Renderer/FrameGraph/FrameGraphPass.hpp"
#include "Waterlily/RHI/CommandBuffer.hpp"
#include "Waterlily/RHI/Types.hpp"

namespace Wl
{

    RHIRenderPassBeginInfo FrameGraphPassExecutionContext::CreateRenderPassBeginInfo(Vector4f color,
                                                                                     Rect2D area,
                                                                                     float depth,
                                                                                     uint32_t stencil)
    {
        RHIRenderPassBeginInfo info = {};
        info.RenderPass = RenderPass;
        info.Framebuffer = Framebuffer;
        info.Color = color;
        info.Area = area;
        info.Depth = depth;
        info.Stencil = stencil;
        return info;
    }

}// namespace Wl
