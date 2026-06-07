#pragma once

#include "Waterlily/Core/Containers/HashMap.hpp"
#include "Waterlily/Core/Containers/ObjectCache.hpp"
#include "Waterlily/Core/Memory/SharedPtr.hpp"
#include "Waterlily/Core/String/StringID.hpp"
#include "Waterlily/RHI/Device.hpp"
#include "Waterlily/RHI/Framebuffer.hpp"

namespace Wl
{

    class FrameGraphFramebufferCache : public ObjectCache<RHIFramebufferDescription, RHIFramebuffer*>
    {
    public:
        FrameGraphFramebufferCache(SharedPtr<RHIDevice> device)
        {
            m_createCallback = [device](const RHIFramebufferDescription& description) mutable -> RHIFramebuffer*
            {
                return device->CreateFramebuffer(description);
            };

            m_destroyCallback = [device](RHIFramebuffer*& framebuffer) mutable -> void
            {
                device->DestroyFramebuffer(framebuffer);
            };
        }
    };

    class FrameGraphRenderPassCache
    {
    public:
        RHIRenderPass* GetRenderPass(StringID name);
        RHIRenderPass* Create(StringID name, const RHIRenderPassDescription& description);
        void Clear();

        FrameGraphRenderPassCache(SharedPtr<RHIDevice> device)
            : m_device(device)
        {
        }

    private:
        SharedPtr<RHIDevice> m_device;
        HashMap<StringID, RHIRenderPass*> m_renderPasses;
    };

}// namespace Wl
