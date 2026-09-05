#include "Waterlily/Renderer/FrameGraph/FrameGraphCache.hpp"

namespace Wl
{

    RHIRenderPass* FrameGraphRenderPassRegistry::GetRenderPass(StringID name)
    {
        if (RHIRenderPass** renderPass = m_renderPasses.GetPtr(name))
        {
            return *renderPass;
        }
        return nullptr;
    }

    RHIRenderPass* FrameGraphRenderPassRegistry::Create(StringID name, const RHIRenderPassDescription& description)
    {
        WL_CHECK(!GetRenderPass(name));
        RHIRenderPass* renderPass = m_device->CreateRenderPass(description);
        m_renderPasses[name] = renderPass;
        return renderPass;
    }

    void FrameGraphRenderPassRegistry::Clear()
    {
        for (auto [_, renderPass]: m_renderPasses)
        {
            m_device->DestroyRenderPass(renderPass);
        }
        m_renderPasses.Clear();
    }

}// namespace Wl