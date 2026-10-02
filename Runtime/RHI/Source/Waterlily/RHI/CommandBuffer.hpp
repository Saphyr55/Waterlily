#pragma once

#include "Waterlily/Core/Containers/Array.hpp"
#include "Waterlily/Core/Containers/ArrayView.hpp"
#include "Waterlily/Core/Containers/Option.hpp"
#include "Waterlily/Core/Math/Vector4.hpp"
#include "Waterlily/RHI/Buffer.hpp"
#include "Waterlily/RHI/RHIForwards.hpp"
#include "Waterlily/RHI/Texture.hpp"
#include "Waterlily/RHI/TextureView.hpp"
#include "Waterlily/RHI/Types.hpp"
#include <cstdint>

namespace Wl
{

    /**
     * @brief Base type for all RHI commands.
     */
    struct RHICommand
    {
    };

    struct RHIDispatchCommand : RHICommand
    {
        uint32 GroupCountX;
        uint32 GroupCountY;
        uint32 GroupCountZ;
    };

    /**
     * @brief Represents a draw command for rendering.
     */
    struct RHIDrawCommand : RHICommand
    {
        uint32 VertexCount;  // Number of vertices to draw.
        uint32 InstanceCount;// Number of instances to draw.
        uint32 FirstVertex;  // Index of the first vertex.
        uint32 FirstInstance;// Index of the first instance.
    };

    struct RHIDrawIndexedCommand : RHICommand
    {
        uint32 IndexCount = 0;
        uint32 InstanceCount = 1;
        uint32 FirstIndex = 0;
        uint32 VertexOffset = 0;
        uint32 FirstInstance = 0;
    };

    struct RHIDrawIndexedIndirectCommand : RHICommand
    {
        RHIBuffer* Buffer;
        usize Offset;
        uint32 DrawCount;
        uint32 Stride;
    };

    struct RHICopyBufferCommand : RHICommand
    {
        RHIBuffer* Source;
        usize SourceOffset = 0;
        RHIBuffer* Destination;
        usize DestinationOffset = 0;
        usize Size = 0;
    };

    struct RHICopyBufferToTextureCommand : RHICommand
    {
        RHIBuffer* Source;
        RHITexture* Destination;
    };
    
    struct RHIBlitTextureCommand : RHICommand
    {
        RHITexture* Source;
        RHITexture* Destination;
        RHITextureLayout SourceLayout = RHITextureLayout::TransferSrc;
        RHITextureLayout DestinationLayout = RHITextureLayout::TransferDst;
        uint32 Width = 2;
        uint32 Height = 2;
        RHIFilter Filter = RHIFilter::Linear;
    };

    struct RHIShaderConstants
    {
        const void* Data = nullptr;
        uint32 Size = 0;
        uint32 Offset = 0;
        RHIShaderStage Stage = RHIShaderStage::AllGraphics;

        RHIShaderConstants() = default;
        RHIShaderConstants(const void* data,
                           uint32 size,
                           uint32 offset = 0,
                           RHIShaderStage stage = RHIShaderStage::AllGraphics)
            : Data(data)
            , Size(size)
            , Offset(offset)
            , Stage(stage)
        {
        }
    };

    struct RHIBufferMemoryBarrier
    {
        RHIBuffer* Buffer;
        RHIBufferUsageFlags SourceUsage;
        RHIBufferUsageFlags DestinationUsage;
        usize Offset;
        usize Size;
    };

    struct RHITextureBarrier
    {
        RHITexture* Texture;

        RHITextureLayout OldLayout;
        RHITextureLayout NewLayout;

        RHITextureUsageFlags SourceUsage;
        RHITextureUsageFlags DestinationUsage;

        uint32 BaseMip = 0;
        uint32 LevelCount = 0;

        uint32 BaseLayer = 0;
        uint32 LayerCount = 0;
    };

    /**
     * @brief Information required to begin a render pass.
     */
    struct RHIRenderPassBeginInfo
    {
        RHIRenderPass* RenderPass = nullptr;
        RHIFramebuffer* Framebuffer = nullptr;
        Rect2D Area = {};
        Vector4f Color = Vector4f(0.0f);
        float Depth = 1.0f;
        uint32 Stencil = 0;
    };

    struct RHIRenderingAttachmentInfo
    {
        Vector4f ClearValue;
        RHIAttachmentLoadOp LoadOp = RHIAttachmentLoadOp::Clear;
        RHIAttachmentStoreOp StoreOp = RHIAttachmentStoreOp::Store;
        RHITextureView* TextureView;
        RHITextureLayout TextureLayout;
    };

    struct RHIBeginRenderingInfo
    {
        Array<RHIRenderingAttachmentInfo> ColorAttachments;
        Option<RHIRenderingAttachmentInfo> DepthAttachment = Option<RHIRenderingAttachmentInfo>::None();
        Option<RHIRenderingAttachmentInfo> StencilAttachment = Option<RHIRenderingAttachmentInfo>::None();
        Rect2D RenderArea;
        uint32 LayerCount = 1;
    };

    /**
     * @brief Abstract interface representing a command buffer for recording GPU commands.
     */
    class RHICommandBuffer
    {
    public:
        /**
         * @brief Begin recording commands into this command buffer.
         */
        virtual void Begin(RHICommandBufferUsageFlags usage = RHICommandBufferUsageFlags::None) = 0;

        /**
         * @brief End recording commands into this command buffer.
         */
        virtual void End() = 0;

        virtual void BeginRendering(const RHIBeginRenderingInfo& info) = 0;
        virtual void EndRendering() = 0;

        /**
         * @brief Begin a render pass.
         * @param render_pass_beginfo Information describing the render pass begin.
         */
        virtual void BeginRenderPass(const RHIRenderPassBeginInfo& info) = 0;

        /**
         * @brief End the current render pass.
         */
        virtual void EndRenderPass() = 0;

        /**
         * @brief Bind a graphics pipeline for subsequent draw calls.
         * @param pipeline Handle to the pipeline to bind.
         */
        virtual void BindPipeline(RHIPipeline* pipeline) = 0;

        virtual void BindSRG(RHIPipeline* pipeline, const Array<RHIShaderResourceGroup*>& groups, usize groupIndex) = 0;

        virtual void SetShaderConstants(RHIPipeline* pipeline, const RHIShaderConstants& constants) = 0;

        /**
         * @brief Bind vertex buffers for rendering.
         * @param buffers Array of buffer handles to bind.
         */
        virtual void BindVertexBuffers(ArrayView<RHIBuffer*> buffers) = 0;

        // TODO: Make the index type configurable
        virtual void BindIndexBuffer(RHIBuffer* buffer) = 0;

        /**
         * @brief Set scissors for rendering.
         * @param scissors Pointer to an array of scissor rectangles.
         * @param count Number of scissor rectangles.
         */
        virtual void SetScissors(const Rect2D* scissors, uint32 count) = 0;
        virtual void SetScissor(const Rect2D& scissor)
        {
            SetScissors(&scissor, 1);
        }

        /**
         * @brief Set viewports for rendering.
         * @param viewports Pointer to an array of viewports.
         * @param count Number of viewports.
         */
        virtual void SetViewports(const Viewport* viewports, uint32 count) = 0;
        virtual void SetViewport(const Viewport& viewport)
        {
            SetViewports(&viewport, 1);
        }

        virtual void TextureGenerateMipmap(const RHITextureLayoutTransition& transition) = 0;
        virtual void TransitionTextureLayout(const RHITextureLayoutTransition& transition) = 0;

        virtual void PipelineBarrier(ArrayView<RHIBufferMemoryBarrier> barriers) = 0;
        virtual void PipelineBarrier(ArrayView<RHITextureBarrier> barriers) = 0;

        virtual void CopyBuffer(const RHICopyBufferCommand& command) = 0;
        virtual void CopyBufferToTexture(const RHICopyBufferToTextureCommand& command) = 0;

        /**
         * @brief Register a draw command.
         * @param command Draw command parameters.
         */
        virtual void Draw(const RHIDrawCommand& command) = 0;
        virtual void Draw(const RHIDrawIndexedCommand& command) = 0;
        virtual void Draw(const RHIDrawIndexedIndirectCommand& command) = 0;

        virtual void Dispatch(const RHIDispatchCommand& command) = 0;

        virtual void BlitTexture(const RHIBlitTextureCommand& command) = 0;

        /**
         * @brief Destructor.
         */
        virtual ~RHICommandBuffer() = default;
    };

    struct RHICommandAllocatorDescription
    {
        SharedPtr<RHICommandQueue> CommandQueue;
        uint32 Count = 1;
    };

    /**
     * @brief Abstract interface for allocating and managing command buffers.
     */
    class RHICommandAllocator
    {
    public:
        /**
         * @brief Open a command buffer for recording.
         * @param index Index identifying which buffer to open.
         * @return Handle to the opened command buffer.
         */
        virtual RHICommandBuffer* OpenCommandBuffer(uint32 index = 0) = 0;

        /**
         * @brief Reset a previously recorded command buffer for reuse.
         * @param commandBuffer Handle to the command buffer to reset.
         */
        virtual void ResetCommandBuffer(RHICommandBuffer* commandBuffer) = 0;

        /**
         * @brief Destructor.
         */
        virtual ~RHICommandAllocator() = default;
    };

}// namespace Wl
