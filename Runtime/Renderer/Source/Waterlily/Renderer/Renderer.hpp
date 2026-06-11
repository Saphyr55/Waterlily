#pragma once

#include "Waterlily/Core/Function/Function.hpp"
#include "Waterlily/Core/Memory/SharedPtr.hpp"
#include "Waterlily/Renderer/FrameContext.hpp"
#include "Waterlily/Renderer/FrameGraph/FrameGraph.hpp"

namespace Wl
{

    class Renderer
    {
    public:
        void RenderFrame();

    public:
        Renderer(const SharedPtr<FrameContext>& frameContext);

    private:
        SharedPtr<FrameContext> m_frameContext;
        FrameGraph m_frameGraph;
    };

}// namespace Wl
