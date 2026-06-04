#pragma once

#include "Waterlily/Core/Containers/Array.hpp"
#include "Waterlily/Core/Containers/ArrayView.hpp"
#include "Waterlily/Core/Defines.hpp"
#include "Waterlily/Core/Memory/LinearAllocator.hpp"
#include "Waterlily/RHI/Buffer.hpp"
#include "Waterlily/RHI/CommandBuffer.hpp"
#include "Waterlily/RHI/Types.hpp"
#include "Waterlily/RHIVulkan/VulkanContext.hpp"

#include <vulkan/vulkan_core.h>

namespace Wl
{

    class VulkanCommandBuffer : public RHICommandBuffer
    {
    public:
        virtual void Begin(RHICommandBufferUsageFlags usage = RHICommandBufferUsageFlags::None) override;

        virtual void End() override;

        virtual void BeginRenderPass(const RHIRenderPassBeginInfo& beginInfo) override;

        virtual void EndRenderPass() override;

        virtual void BindPipeline(RHIPipeline* pipeline) override;

        virtual void BindSRG(RHIPipeline* pipeline,
                             const Array<RHIShaderResourceGroup*>& groups,
                             size_t groupIndex) override;

        virtual void SetShaderConstants(RHIPipeline* pipeline, const RHIShaderConstants& shader_constants) override;

        virtual void BindVertexBuffers(const Array<RHIBuffer*>& buffers) override;

        virtual void BindIndexBuffer(RHIBuffer* buffer) override;

        virtual void SetScissors(const Rect2D* scissors, uint32_t count) override;

        virtual void SetViewports(const Viewport* viewports, uint32_t count) override;

        virtual void TextureGenerateMipmap(const RHITextureLayoutTransition& transition) override;

        virtual void TransitionTextureLayout(const RHITextureLayoutTransition& transition) override;

        virtual void PipelineBarrier(ArrayView<RHIBufferMemoryBarrier> barriers) override;

        virtual void PipelineBarrier(ArrayView<RHITextureBarrier> barriers) override;

        virtual void CopyBufferToTexture(const RHICopyBufferToTextureCommand& command) override;

        virtual void CopyBuffer(const RHICopyBufferCommand& command) override;

        virtual void Draw(const RHIDrawCommand& command) override;

        virtual void Draw(const RHIDrawIndexedCommand& command) override;

        virtual void Draw(const RHIDrawIndexedIndirectCommand& command) override;

        inline VkCommandBuffer& GetHandle()
        {
            return m_handle;
        }

    public:
        VulkanCommandBuffer(VkCommandBuffer commandBuffer, RHIQueueType queueType)
            : m_queueType(queueType)
            , m_handle(commandBuffer)
        {
        }
        ~VulkanCommandBuffer() = default;

    private:
        RHIQueueType m_queueType;
        VkCommandBuffer m_handle;
    };

    class VulkanCommandAllocator : public RHICommandAllocator
    {
    public:
        virtual RHICommandBuffer* OpenCommandBuffer(uint32_t index = 0) override;

        virtual void ResetCommandBuffer(RHICommandBuffer* command_buffer) override;

    public:
        VkCommandPool& GetHandle();

        const Array<VkCommandBuffer>& GetCommandBuffers() const;

        void Create();

        void AllocateCommandBuffers();

        void Destroy();

    public:
        VulkanCommandAllocator(VulkanContext& context, const RHICommandAllocatorDescription& description);
        ~VulkanCommandAllocator() = default;

    private:
        VkCommandPool m_handle;
        Array<VulkanCommandBuffer*> m_upperCommandBuffers;
        Array<VkCommandBuffer> m_commandBuffers;
        RHIQueueType m_queueType;
        uint32_t m_queueFamilyIndex = 0;
        uint32_t m_count = 0;
        LinearAllocator m_upperCommandBufferAllocator;

        VulkanContext& m_context;
    };

}// namespace Wl