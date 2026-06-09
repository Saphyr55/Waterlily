#pragma once

#include "Waterlily/Core/Memory/HeapAllocator.hpp"
#include "Waterlily/Core/Memory/SharedPtr.hpp"
#include "Waterlily/RHI/Buffer.hpp"
#include "Waterlily/RHI/BufferPool.hpp"
#include "Waterlily/RHI/CommandBuffer.hpp"
#include "Waterlily/RHI/Device.hpp"
#include "Waterlily/RHI/Fence.hpp"
#include "Waterlily/RHI/Framebuffer.hpp"
#include "Waterlily/RHI/RenderPass.hpp"
#include "Waterlily/RHI/Sampler.hpp"
#include "Waterlily/RHI/ShaderResource.hpp"
#include "Waterlily/RHI/Swapchain.hpp"
#include "Waterlily/RHI/TexturePool.hpp"
#include "Waterlily/RHIVulkan/VulkanContext.hpp"

#undef CreateSemaphore

namespace Wl
{

    class VulkanDevice : public RHIDevice
    {
    public:
        virtual size_t GetCountBufferAllocation() override
        {
            return m_countBufferAllocation;
        }

        virtual size_t GetCountTextureAllocation() override
        {
            return m_countTextureAllocation;
        }

        virtual void Init(void* nativeWindow) override;
        virtual void Shutdown() override;

        virtual void WaitIdle() override;

        virtual void WaitFence(RHIFence* fence) override;
        virtual void ResetFence(RHIFence* fence) override;

        virtual const RHIDeviceProperties& GetDeviceProperties() const override;

        virtual RHIBindlessShaderResources* CreateBindlessShaderResources(
                uint32_t max_resources,
                const Array<RHIShaderResourceBinding>& bindings) override;
        virtual void DestroyBindlessShaderResources(RHIBindlessShaderResources* group) override;

        virtual RHIShaderResourceGroupLayout* CreateSRGLayout(
                const Array<RHIShaderResourceBinding>& bindings) override;
        virtual void DestroySRGLayout(RHIShaderResourceGroupLayout* layout) override;

        virtual RHIShaderResourceGroupPool* CreateSRGPool(
                size_t max_groups,
                const Array<RHIShaderResourceBinding>& total_bindings) override;
        virtual void DestroySRGPool(RHIShaderResourceGroupPool* descriptor_set_layout) override;

        virtual RHICommandAllocator* CreateCommandAllocatorr(const RHICommandAllocatorDescription& description) override;
        virtual void DestroyCommandAllocator(RHICommandAllocator* command_allocator) override;

        virtual RHITexture* CreateTexturre(const RHITextureDescription& description) override;
        virtual void DestroyTexture(RHITexture* texture) override;

        virtual RHIBuffer* CreateBuffer(const RHIBufferDescription& description) override;
        virtual void DestroyBuffer(RHIBuffer* buffer) override;

        virtual RHITextureView* CreateTextureView(const RHITextureViewDescription& description) override;
        virtual void DestroyTextureView(RHITextureView* texture_view) override;

        virtual RHISampler* CreateSampler(const RHISamplerDescription& description) override;
        virtual void DestroySampler(RHISampler* sampler) override;

        virtual RHIRenderPass* CreateRenderPass(const RHIRenderPassDescription& description) override;
        virtual void DestroyRenderPass(RHIRenderPass* render_pass) override;

        virtual RHIGraphicsPipeline* CreateGraphicsPipeline(const RHIGraphicsPipelineDescription& description) override;
        virtual void DestroyGraphicsPipeline(RHIGraphicsPipeline* pipeline) override;

        virtual RHISwapchain* CreateSwapchain(uint32_t width, uint32_t height, uint32_t image_count) override;
        virtual void RecreateSwapchain(RHISwapchain* swapchain, uint32_t width, uint32_t height) override;
        virtual void DestroySwapchain(RHISwapchain* swapchain) override;

        virtual RHIFramebuffer* CreateFramebuffer(const RHIFramebufferDescription& description) override;
        virtual void DestroyFramebuffer(RHIFramebuffer* framebuffer) override;

        virtual RHISemaphore* CreateSemaphore() override;
        virtual void DestroySemaphore(RHISemaphore* semaphore) override;

        virtual RHIFence* CreateFence() override;
        virtual void DestroyFence(RHIFence* fence) override;

        virtual Array<SharedPtr<RHICommandQueue>> GetCommandQueues() override;

    public:
        VulkanDevice();
        ~VulkanDevice() override
        {
        }

    private:
        RHIDeviceProperties m_properties;
        HeapAllocator& m_allocator;
        VulkanContext& m_context;
        size_t m_countTextureAllocation = 0;
        size_t m_countBufferAllocation = 0;
    };

}// namespace Wl
