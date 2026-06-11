#include "Waterlily/Renderer/Renderer.hpp"
#include "Renderer.hpp"

namespace Wl
{

    void Renderer::RenderFrame()
    {
        Frame& frame = m_frameContext->GetCurrentFrame();

    }

    Renderer::Renderer(const SharedPtr<FrameContext>& frameContext)
        : m_frameContext(frameContext)
        , m_frameGraph(frameContext)
    {
    }

}// namespace Wl