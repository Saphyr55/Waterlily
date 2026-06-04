#include "Waterlily/RHI/Texture.hpp"
#include "Waterlily/RHI/CommandBuffer.hpp"
#include "Waterlily/RHI/CommandQueue.hpp"
#include "Waterlily/RHI/Device.hpp"

namespace Wl
{

    const RHITextureDescription& RHITexture::GetDescription() const
    {
        return m_description;
    }

    void RHITransitionTexture(SharedPtr<RHIDevice> device,
                              const RHITextureLayoutTransition& barrier,
                              const SharedPtr<RHICommandQueue>& queue)
    {
        RHICommandAllocatorDescription commandAllocatorDescription = {};
        commandAllocatorDescription.CommandQueue = queue;

        RHICommandAllocator* commandAllocator = device->CreateCommandAllocatorr(commandAllocatorDescription);

        // Open a transfer command buffer
        RHICommandBuffer* commandBuffer = commandAllocator->OpenCommandBuffer();
        commandAllocator->ResetCommandBuffer(commandBuffer);

        commandBuffer->Begin();
        commandBuffer->TransitionTextureLayout(barrier);
        commandBuffer->End();

        device->DestroyCommandAllocator(commandAllocator);
    }

}// namespace Wl
