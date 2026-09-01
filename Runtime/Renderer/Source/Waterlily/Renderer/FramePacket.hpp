#pragma once

#include "Waterlily/Core/Containers/ArrayView.hpp"
#include "Waterlily/Renderer/RenderAllocator.hpp"

namespace Wl
{

    struct FramePacket
    {
        ArrayView<RHIBuffer*> VertexBuffers;
        RHIBuffer* IndexBuffers;
        
        RenderAllocation ViewAllocation;

        RenderAllocation InstanceAllocation;

        RenderAllocation PointLightsAllocation;
        RenderAllocation DirectionalLightAllocation;
        RenderAllocation CountersAllocation;
        
        uint32_t DrawCount = 0;
    };

}// namespace Wl
