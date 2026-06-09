#pragma once

#include "Waterlily/Core/Containers/Array.hpp"
#include "Waterlily/Core/Containers/HashMap.hpp"
#include "Waterlily/Core/Containers/HashSet.hpp"
#include "Waterlily/Core/Logging/Trace.hpp"
#include "Waterlily/Core/Memory/SharedPtr.hpp"
#include "Waterlily/Core/String/StringID.hpp"
#include "Waterlily/RHI/Buffer.hpp"
#include "Waterlily/RHI/CommandBuffer.hpp"
#include "Waterlily/RHI/Device.hpp"
#include "Waterlily/RHI/Framebuffer.hpp"
#include "Waterlily/RHI/RHIForwards.hpp"
#include "Waterlily/RHI/RenderPass.hpp"
#include "Waterlily/RHI/Texture.hpp"
#include "Waterlily/RHI/TextureView.hpp"
#include "Waterlily/Renderer/FrameContext.hpp"
#include "Waterlily/Renderer/FrameGraph/FrameGraphCache.hpp"
#include "Waterlily/Renderer/FrameGraph/FrameGraphPass.hpp"
#include "Waterlily/Renderer/FrameGraph/FrameGraphResource.hpp"
#include "Waterlily/Renderer/FrameGraph/FrameGraphResourcePool.hpp"
#include "Waterlily/Renderer/RendererExports.hpp"

namespace Wl
{

    class WL_RENDERER_API FrameGraph
    {
    public:
        FrameGraphTextureHandle CreateTexture(const FrameGraphTextureInfo& info);
        FrameGraphBufferHandle CreateBuffer(const FrameGraphBufferInfo& info);

        FrameGraphTextureHandle ImportTexture(RHITexture* texture, RHITextureView* view);
        FrameGraphBufferHandle ImportBuffer(RHIBuffer* buffer, size_t range, size_t offset);

        void UpdateImportedTexture(FrameGraphTextureHandle handle, RHITexture* texture, RHITextureView* view);
        void UpdateImportedBuffer(FrameGraphBufferHandle handle, RHIBuffer* buffer, size_t range, size_t offset);

        void AddOutput(FrameGraphTextureHandle& handle);

        FrameGraphPass& AddPass(const StringID& name);
        FrameGraphPass& GetPass(const StringID& name);

        template<typename PassType>
        PassType& GetPass(const StringID& name)
        {
            FrameGraphPass& pass = GetPass(name);
            SharedPtr<PassType> delegatePass = pass.GetDelegate<PassType>();
            WL_CHECK_MSG(delegatePass, "Failed to cast pass %s to the specified type!", name.GetText().data());
            return *delegatePass;
        }

        RHIRenderPass* GetRenderPass(const FrameGraphPass& pass);
        RHIRenderPass* GetRenderPass(const StringID& name);

        void BeginFrame();
        void EndFrame();

        void Compile();
        void Execute(RHICommandBuffer* command_buffer);

        void Dispose();

        FrameGraphPhysicalTexture& ResolvePhysicalTexture(const FrameGraphTextureHandle& handle);

        inline FrameGraphTextureResource& GetTexture(const FrameGraphTextureHandle& handle)
        {
            return m_textures[handle.GetIndex()];
        }

        inline FrameGraphBufferResource& GetBuffer(const FrameGraphBufferHandle& handle)
        {
            return m_buffers[handle.GetIndex()];
        }

        inline const FrameGraphTextureResource& GetTexture(const FrameGraphTextureHandle& handle) const
        {
            return m_textures[handle.GetIndex()];
        }

        inline const FrameGraphBufferResource& GetBuffer(const FrameGraphBufferHandle& handle) const
        {
            return m_buffers[handle.GetIndex()];
        }

        inline const Array<size_t>& GetSortedPasses() const
        {
            return m_sortedPasses;
        }

        inline Array<size_t> GetSortedPasses()
        {
            return m_sortedPasses;
        }

        inline Array<FrameGraphPass>& GetPasses()
        {
            return m_passes;
        }

        inline Array<FrameGraphTextureResource>& GetTextures()
        {
            return m_textures;
        }

        inline Array<FrameGraphBufferResource>& GetBuffers()
        {
            return m_buffers;
        }

    public:
        FrameGraph(const SharedPtr<RHIDevice>& device, const SharedPtr<FrameContext>& frame_context);
        FrameGraph(const FrameGraph&) = delete;
        FrameGraph(FrameGraph&&) = default;
        ~FrameGraph() = default;

    private:
        void BuildDependencies();
        void TopoligicalSort();
        void BuildBarriers();
        void ComputeResourceLifetimes();
        void BuildPasses();

        void AllocatePhysicalPassResources(size_t pass_index);
        void DeallocatePhysicalPassResources(size_t pass_index);

        void AllocatePhysicalResource(FrameGraphTextureResource& resource);
        void DeallocatePhysicalResource(FrameGraphTextureResource& resource);

        RHIFramebuffer* BuildFramebuffer(FrameGraphPass& pass);

        void DestroyPasses();

        RHITextureLayoutTransition BarrierToRHITransition(const FrameGraphTextureBarrier& barrier);

        bool IsOutputResource(FrameGraphTextureHandle handle);

    private:
        SharedPtr<RHIDevice> m_device;
        SharedPtr<FrameContext> m_frameContext;

        Array<FrameGraphTextureResource> m_textures;
        Array<FrameGraphBufferResource> m_buffers;

        HashSet<FrameGraphTextureHandle> m_outputs;

        HashMap<StringID, size_t> m_passNames;
        Array<FrameGraphPass> m_passes;
        Array<size_t> m_sortedPasses;

        FrameGraphPhysicalTexturePool m_texturePool;

        FrameGraphFramebufferCache m_framebufferCache;
        FrameGraphRenderPassCache m_renderPassCache;
    };

}// namespace Wl