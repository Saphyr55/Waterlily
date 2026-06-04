#pragma once

#include "Waterlily/Core/Containers/Array.hpp"
#include "Waterlily/Core/Containers/ArrayView.hpp"
#include "Waterlily/Core/Defines.hpp"
#include "Waterlily/Core/Math/Vector4.hpp"
#include "Waterlily/RHI/Buffer.hpp"
#include "Waterlily/RHI/RHIForwards.hpp"
#include "Waterlily/RHI/Texture.hpp"
#include "Waterlily/RHI/Types.hpp"

namespace Wl
{

    /**
 * @brief Base type for all RHI commands.
 */
    struct RHICommand
    {
    };

    /**
 * @brief Represents a draw command for rendering.
 */
    struct RHIDrawCommand : RHICommand
    {
        uint32_t VertexCount;  // Number of vertices to draw.
        uint32_t InstanceCount;// Number of instances to draw.
        uint32_t FirstVertex;  // Index of the first vertex.
        uint32_t FirstInstance;// Index of the first instance.
    };

    struct RHIDrawIndexedCommand : RHICommand
    {
        uint32_t IndexCount = 0;
        uint32_t InstanceCount = 1;
        uint32_t FirstIndex = 0;
        uint32_t VertexOffset = 0;
        uint32_t FirstInstance = 0;
    };

    struct RHIDrawIndexedIndirectCommand : RHICommand
    {
        RHIBuffer* Buffer;
        size_t Offset;
        uint32_t DrawCount;
        uint32_t Stride;
    };

    struct RHICopyBufferCommand : RHICommand
    {
        RHIBuffer* Source;
        size_t SourceOffset = 0;
        RHIBuffer* Destination;
        size_t DestinationOffset = 0;
        size_t Size = 0;
    };

    struct RHICopyBufferToTextureCommand : RHICommand
    {
        RHIBuffer* Source;
        RHITexture* Destination;
    };

    struct RHIShaderConstants
    {
        const void* Data = nullptr;
        uint32_t Size = 0;
        uint32_t Offset = 0;
        RHIShaderStage Stage = RHIShaderStage::AllGraphics;

        RHIShaderConstants() = default;
        RHIShaderConstants(const void* data,
                           uint32_t size,
                           uint32_t offset = 0,
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
        size_t Offset;
        size_t Size;
    };

    struct RHITextureBarrier
    {
        RHITexture* Texture;

        RHITextureLayout OldLayout;
        RHITextureLayout NewLayout;

        RHITextureUsageFlags SourceUsage;
        RHITextureUsageFlags DestinationUsage;

        uint32_t BaseMip = 0;
        uint32_t LevelCount = 0;

        uint32_t BaseLayer = 0;
        uint32_t LayerCount = 0;
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
        uint32_t Stencil = 0;
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

        /**
     * @brief Begin a render pass.
     * @param render_pass_beginfo Information describing the render pass begin.
     */
        virtual void BeginRenderPass(const RHIRenderPassBeginInfo& render_pass_beginfo) = 0;

        /**
     * @brief End the current render pass.
     */
        virtual void EndRenderPass() = 0;

        /**
     * @brief Bind a graphics pipeline for subsequent draw calls.
     * @param pipeline Handle to the pipeline to bind.
     */
        virtual void BindPipeline(RHIPipeline* pipeline) = 0;

        virtual void BindSRG(RHIPipeline* pipeline, const Array<RHIShaderResourceGroup*>& groups, size_t groupIndex) = 0;

        virtual void SetShaderConstants(RHIPipeline* pipeline, const RHIShaderConstants& constants) = 0;

        /**
     * @brief Bind vertex buffers for rendering.
     * @param buffers Array of buffer handles to bind.
     */
        virtual void BindVertexBuffers(const Array<RHIBuffer*>& buffers) = 0;

        // TODO: Make the index type configurable
        virtual void BindIndexBuffer(RHIBuffer* buffer) = 0;

        /**
     * @brief Set scissors for rendering.
     * @param scissors Pointer to an array of scissor rectangles.
     * @param count Number of scissor rectangles.
     */
        virtual void SetScissors(const Rect2D* scissors, uint32_t count) = 0;
        virtual void SetScissor(const Rect2D& scissor)
        {
            SetScissors(&scissor, 1);
        }

        /**
     * @brief Set viewports for rendering.
     * @param viewports Pointer to an array of viewports.
     * @param count Number of viewports.
     */
        virtual void SetViewports(const Viewport* viewports, uint32_t count) = 0;
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

        /**
     * @brief Destructor.
     */
        virtual ~RHICommandBuffer() = default;
    };

    struct RHICommandAllocatorDescription
    {
        SharedPtr<RHICommandQueue> CommandQueue;
        uint32_t Count = 1;
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
        virtual RHICommandBuffer* OpenCommandBuffer(uint32_t index = 0) = 0;

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
