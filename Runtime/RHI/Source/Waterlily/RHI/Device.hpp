#pragma once

#include "BindlessShaderResources.hpp"
#include "Waterlily/Core/Function/Function.hpp"
#include "Waterlily/Core/Memory/SharedPtr.hpp"
#include "Waterlily/RHI/Buffer.hpp"
#include "Waterlily/RHI/BufferPool.hpp"
#include "Waterlily/RHI/CommandBuffer.hpp"
#include "Waterlily/RHI/CommandQueue.hpp"
#include "Waterlily/RHI/Framebuffer.hpp"
#include "Waterlily/RHI/GraphicsPipeline.hpp"
#include "Waterlily/RHI/RHIForwards.hpp"
#include "Waterlily/RHI/RenderPass.hpp"
#include "Waterlily/RHI/Sampler.hpp"
#include "Waterlily/RHI/ShaderResource.hpp"
#include "Waterlily/RHI/TexturePool.hpp"
#include "Waterlily/RHI/TextureView.hpp"

namespace Wl
{

    struct RHIDeviceProperties
    {
        size_t MinUniformBufferOffsetAlignment = 0;
        size_t MinStorageBufferOffsetAlignment = 0;
        size_t MaxUniformBufferRange = 0;
        size_t MaxStorageBufferRange = 0;
        size_t MaxBoundShaderResourceGroup = 0;
        size_t MaxComputeWorkGroupInvocations = 0;
        size_t NonCoherentAtomSize = 0;
    };

    class RHIDevice
    {
    public:
        virtual size_t GetCountBufferAllocation() = 0;
        virtual size_t GetCountTextureAllocation() = 0;

        virtual void Init(void* nativeWindow) = 0;
        virtual void Destroy() = 0;

        virtual void WaitIdle() = 0;

        virtual void WaitFence(RHIFence* fence) = 0;
        virtual void ResetFence(RHIFence* fence) = 0;

        virtual const RHIDeviceProperties& GetDeviceProperties() const = 0;

        virtual RHIBindlessShaderResources* CreateBindlessShaderResources(
                uint32_t maxResources,
                const Array<RHIShaderResourceBinding>& bindings) = 0;
        virtual void DestroyBindlessShaderResources(RHIBindlessShaderResources* bindless) = 0;

        virtual RHIShaderResourceGroupLayout* CreateSRGLayout(
                const Array<RHIShaderResourceBinding>& bindings) = 0;
        virtual void DestroySRGLayout(RHIShaderResourceGroupLayout* layout) = 0;

        virtual RHIShaderResourceGroupPool* CreateSRGPool(
                size_t maxGroups,
                const Array<RHIShaderResourceBinding>& totalBindings) = 0;
        virtual void DestroySRGPool(RHIShaderResourceGroupPool* pool) = 0;

        virtual RHICommandAllocator* CreateCommandAllocatorr(const RHICommandAllocatorDescription& description) = 0;
        virtual void DestroyCommandAllocator(RHICommandAllocator* command_allocator) = 0;

        virtual RHITexture* CreateTexturre(const RHITextureDescription& description) = 0;
        virtual void DestroyTexture(RHITexture* texture) = 0;

        virtual RHIBuffer* CreateBuffer(const RHIBufferDescription& description) = 0;
        inline RHIBuffer* CreateIndirectBuffer(ArrayView<RHIDrawIndexedCommand> commands)
        {
            return CreateBuffer(RHIBufferDescription {
                    .Size = sizeof(RHIDrawIndexedCommand) * commands.GetSize(),
                    .Usage = RHIBufferUsageFlags::Indirect | RHIBufferUsageFlags::Storage | RHIBufferUsageFlags::TransferDst,
                    .MemoryUsage = RHIMemoryUsage::Device,
                    .SharingMode = RHISharingMode::Shared,
            });
        }
        virtual void DestroyBuffer(RHIBuffer* buffer) = 0;

        virtual RHITextureView* CreateTextureView(const RHITextureViewDescription& description) = 0;
        virtual void DestroyTextureView(RHITextureView* view) = 0;

        virtual RHISampler* CreateSampler(const RHISamplerDescription& description) = 0;
        virtual void DestroySampler(RHISampler* sampler) = 0;

        virtual RHIRenderPass* CreateRenderPass(const RHIRenderPassDescription& description) = 0;
        virtual void DestroyRenderPass(RHIRenderPass* renderPass) = 0;

        virtual RHIGraphicsPipeline* CreateGraphicsPipeline(const RHIGraphicsPipelineDescription& description) = 0;
        virtual void DestroyGraphicsPipeline(RHIGraphicsPipeline* pipeline) = 0;

        virtual RHISwapchain* CreateSwapchain(uint32_t width, uint32_t height, uint32_t imageCount) = 0;
        virtual void RecreateSwapchain(RHISwapchain* swapchain, uint32_t width, uint32_t height) = 0;
        virtual void DestroySwapchain(RHISwapchain* swapchain) = 0;

        virtual RHIFramebuffer* CreateFramebuffer(const RHIFramebufferDescription& description) = 0;
        virtual void DestroyFramebuffer(RHIFramebuffer* framebuffer) = 0;

        virtual RHISemaphore* CreateSemaphore() = 0;
        virtual void DestroySemaphore(RHISemaphore* semaphore) = 0;

        virtual RHIFence* CreateFence() = 0;
        virtual void DestroyFence(RHIFence* fence) = 0;

        virtual Array<SharedPtr<RHICommandQueue>> GetCommandQueues() = 0;

        inline SharedPtr<RHICommandQueue> GetGraphicsQueue()
        {
            return GetQueueIf([](const SharedPtr<RHICommandQueue>& command_queue) -> bool
            {
                return command_queue->IsGraphicsQueueType();
            });
        }

        inline SharedPtr<RHICommandQueue> GetPresentQueue()
        {
            return GetQueueIf([](const SharedPtr<RHICommandQueue>& command_queue) -> bool
            {
                return command_queue->IsPresentMode();
            });
        }

        inline SharedPtr<RHICommandQueue> GetTransferQueue()
        {
            return GetQueueIf([](const SharedPtr<RHICommandQueue>& command_queue) -> bool
            {
                return command_queue->IsTransferQueueType();
            });
        }

        inline SharedPtr<RHICommandQueue> GetQueueIf(auto&& predicate)
        {
            return *GetCommandQueues().GetIf(predicate);
        }

        void ImediateSubmit(const Function<void(RHICommandBuffer*)>& recordFunc, SharedPtr<RHICommandQueue> queue)
        {
            RHICommandAllocatorDescription desc = {};
            desc.Count = 1;
            desc.CommandQueue = queue;

            RHICommandAllocator* cmdAlloc = CreateCommandAllocatorr(desc);

            RHICommandBuffer* cmdBuffer = cmdAlloc->OpenCommandBuffer();
            cmdAlloc->ResetCommandBuffer(cmdBuffer);

            cmdBuffer->Begin();
            recordFunc(cmdBuffer);
            cmdBuffer->End();

            RHIFence* uploadFence = CreateFence();
            ResetFence(uploadFence);
            queue->Submit({cmdBuffer}, {}, {}, uploadFence);

            WaitFence(uploadFence);
            DestroyFence(uploadFence);

            DestroyCommandAllocator(cmdAlloc);
        }

        virtual ~RHIDevice() = default;
    };

}// namespace Wl