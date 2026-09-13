#include "Waterlily/Renderer/FrameGraph/FrameGraph.hpp"
#include "Waterlily/Core/Containers/HashMap.hpp"
#include "Waterlily/Core/Containers/Option.hpp"
#include "Waterlily/Core/Defines.hpp"
#include "Waterlily/Core/Logging/Trace.hpp"
#include "Waterlily/Core/Math/Math.hpp"
#include "Waterlily/Core/Memory/SharedPtr.hpp"
#include "Waterlily/RHI/CommandBuffer.hpp"
#include "Waterlily/RHI/Device.hpp"
#include "Waterlily/RHI/RHIForwards.hpp"
#include "Waterlily/RHI/Swapchain.hpp"
#include "Waterlily/RHI/Texture.hpp"
#include "Waterlily/RHI/TextureView.hpp"
#include "Waterlily/RHI/Types.hpp"
#include "Waterlily/Renderer/FrameContext.hpp"
#include "Waterlily/Renderer/FrameGraph/FrameGraphPass.hpp"
#include "Waterlily/Renderer/FrameGraph/FrameGraphPassBuilder.hpp"
#include "Waterlily/Renderer/FrameGraph/FrameGraphResource.hpp"

#include <deque>

namespace Wl
{

    FrameGraph::FrameGraph(const SharedPtr<FrameContext>& frameContext)
        : m_device(frameContext->GetDevice())
        , m_frameContext(frameContext)
        , m_texturePool(frameContext)
    {
        for (const RHISwapchainBuffer& buffer: m_frameContext->GetSwapchain()->GetBuffers())
        {
            isFirstFrame[&buffer] = true;
        }
    }

    FrameGraphTextureHandle FrameGraph::CreateTexture(const FrameGraphTextureInfo& info)
    {
        FrameGraphTextureHandle index = m_textures.GetSize();

        m_textures.Append(FrameGraphTextureResource {
                .Info = info,
                .IsTransient = true,
        });

        return index;
    }

    FrameGraphBufferHandle FrameGraph::CreateBuffer(const FrameGraphBufferInfo& info)
    {
        FrameGraphBufferHandle index = m_buffers.GetSize();

        m_buffers.Append(FrameGraphBufferResource {
                .Info = info,
                .PhysicalBuffer = {},
                .IsTransient = true,
        });

        return index;
    }

    FrameGraphTextureHandle FrameGraph::ImportTexture(RHITexture* texture, RHITextureView* view)
    {
        FrameGraphTextureHandle index = m_textures.GetSize();
        m_textures.Append(FrameGraphResource::CreatePersistantResource(texture, view));
        return index;
    }

    FrameGraphBufferHandle FrameGraph::ImportBuffer(RHIBuffer* buffer, size_t range, size_t offset)
    {
        FrameGraphBufferHandle index = m_textures.GetSize();
        m_buffers.Append(FrameGraphResource::CreatePersistantResource(buffer, range, offset));
        return index;
    }

    void FrameGraph::UpdateImportedTexture(FrameGraphTextureHandle handle, RHITexture* texture, RHITextureView* view)
    {
        FrameGraphTextureResource resource = FrameGraphResource::CreatePersistantResource(texture, view);
        m_textures[handle.GetIndex()] = resource;
    }

    void FrameGraph::UpdateImportedBuffer(FrameGraphBufferHandle handle, RHIBuffer* buffer, size_t size, size_t offset)
    {
        FrameGraphBufferResource resource = FrameGraphResource::CreatePersistantResource(buffer, size, offset);
        m_buffers[handle.GetIndex()] = resource;
    }

    void FrameGraph::AddOutput(FrameGraphTextureHandle& handle)
    {
        m_outputs.Add(handle);
        FrameGraphTextureResource& resource = m_textures[handle.GetIndex()];
        // When a texture is an output, we are going to transfer it to the swapchain, and do a blit.
        resource.Usage |= RHITextureUsageFlags::TransferSrc;
    }

    FrameGraphPass& FrameGraph::AddPass(const StringID& name)
    {
        WL_CHECK_MSG(!m_passNames.Contains(name), "A pass named %s already exists in FrameGraph.", name.GetText().data());
        m_passes.Emplace(name);
        m_passNames.Put(name, m_passes.GetSize() - 1);
        return m_passes.Back();
    }

    FrameGraphPass& FrameGraph::GetPass(const StringID& name)
    {
        WL_CHECK_MSG(m_passNames.Contains(name), "No pass named %s found in FrameGraph.", name.GetText().data());
        size_t index = m_passNames[name];
        return m_passes[index];
    }

    void FrameGraph::BeginFrame()
    {
        m_passes.Clear();
        m_passNames.Clear();
        m_textures.Clear();
        m_buffers.Clear();
        m_outputs.Clear();
        m_sortedPasses.Clear();
        m_outputs.Clear();

        m_passRenderingInfos.Clear();

        // TODO: Make it configurable
        uint64_t textureMaxFrameLifetime = 8;
        m_texturePool.GarbageCollect(textureMaxFrameLifetime);
    }

    void FrameGraph::EndFrame()
    {
        const RHISwapchainBuffer& buffer = m_frameContext->GetSwapchain()->GetCurrentBuffer();
        isFirstFrame[&buffer] = false;
    }

    void FrameGraph::Compile()
    {
        WL_CHECK_MSG(m_outputs.GetSize() > 0, "FrameGraph must have at least one output.");

        for (size_t i = 0; i < m_passes.GetSize(); i++)
        {
            FrameGraphPass& pass = m_passes[i];

            pass.m_index = i;

            FrameGraphPassSetupContext context(m_device, m_frameContext, this);
            FrameGraphPassBuilder builder(pass, *this);

            pass.Setup(context, builder);
        }

        BuildDependencies();
        TopoligicalSort();
        WL_CHECK_MSG(m_sortedPasses.GetSize() == m_passes.GetSize(), "The Frame Graph given is not a DAG.");

        ComputeResourceLifetimes();
        ComputeRenderingInfoPasses();

        BuildBarriers();
    }

    void FrameGraph::Execute(RHICommandBuffer* commandBuffer)
    {
        const RHISwapchainBuffer& buffer = m_frameContext->GetSwapchain()->GetCurrentBuffer();

        RHITextureLayoutTransition swapchainTransition = {};
        swapchainTransition.OldLayout = isFirstFrame[&buffer] ? RHITextureLayout::Undefined : RHITextureLayout::Present;
        swapchainTransition.NewLayout = RHITextureLayout::ColorAttachment;
        swapchainTransition.Texture = buffer.Texture;

        commandBuffer->TransitionTextureLayout(swapchainTransition);

        for (size_t orderedPassIndex: m_sortedPasses)
        {
            FrameGraphPass& pass = m_passes[orderedPassIndex];

            AllocatePhysicalPassResources(pass);

            for (FrameGraphTextureBarrier& barrier: pass.m_barriers)
            {
                FrameGraphTextureResource& resource = m_textures[barrier.Handle.GetIndex()];
                if (!resource.IsAllocated)
                {
                    continue;
                }

                RHITextureLayoutTransition transition = BarrierToRHITransition(barrier);
                commandBuffer->TransitionTextureLayout(transition);
            }

            FrameGraphPassExecutionContext context;
            context.FrameContext = m_frameContext;
            context.Device = m_device;
            context.FrameGraph = this;
            context.CommandBuffer = commandBuffer;
            context.Pass = &pass;

            if (pass.IsComputeStage())
            {
                pass.Execute(context);
            }
            else
            {
                RHIBeginRenderingInfo beginRenderingInfo = BuildRenderingInfo(pass);
                commandBuffer->BeginRendering(beginRenderingInfo);
                pass.Execute(context);
                commandBuffer->EndRendering();
            }

            DeallocatePhysicalPassResources(pass);
        }

        if (!m_outputs.IsEmpty())
        {
            swapchainTransition = {};
            swapchainTransition.OldLayout = RHITextureLayout::ColorAttachment;
            swapchainTransition.NewLayout = RHITextureLayout::TransferDst;
            swapchainTransition.Texture = buffer.Texture;
            commandBuffer->TransitionTextureLayout(swapchainTransition);
        }

        for (FrameGraphTextureHandle output: m_outputs)
        {
            FrameGraphTextureResource& resource = m_textures[output.GetIndex()];
            FrameGraphPhysicalTexture& texture = ResolvePhysicalTexture(output);
            RHITextureLayout oldLayout = resource.CurrentLayout;

            RHITextureLayoutTransition transition = {};
            transition.Texture = texture.Texture;
            transition.OldLayout = resource.CurrentLayout;
            transition.NewLayout = RHITextureLayout::TransferSrc;
            commandBuffer->TransitionTextureLayout(transition);

            RHIBlitTextureCommand blitCommand = {};
            blitCommand.Source = texture.Texture;
            blitCommand.Destination = buffer.Texture;
            blitCommand.Width = m_frameContext->GetWidth();
            blitCommand.Height = m_frameContext->GetHeight();
            commandBuffer->BlitTexture(blitCommand);

            transition = {};
            transition.Texture = texture.Texture;
            transition.OldLayout = RHITextureLayout::TransferSrc;
            transition.NewLayout = oldLayout;
            commandBuffer->TransitionTextureLayout(transition);
        }

        swapchainTransition = {};
        swapchainTransition.OldLayout = m_outputs.IsEmpty() ? RHITextureLayout::ColorAttachment : RHITextureLayout::TransferDst;
        swapchainTransition.NewLayout = RHITextureLayout::Present;
        swapchainTransition.Texture = buffer.Texture;
        commandBuffer->TransitionTextureLayout(swapchainTransition);
    }

    void FrameGraph::Resize()
    {
        Destroy();
    }

    void FrameGraph::Destroy()
    {
        for (const RHISwapchainBuffer& buffer: m_frameContext->GetSwapchain()->GetBuffers())
        {
            isFirstFrame[&buffer] = true;
        }

        m_device->WaitIdle();
        m_texturePool.Dispose();
        DestroyPasses();
    }

    void FrameGraph::BuildDependencies()
    {
        HashMap<uint32_t, FrameGraphPass*> lastTextureProducer;
        HashMap<uint32_t, FrameGraphPass*> lastBufferProducer;

        using LastProducerIterator = HashMap<uint32_t, FrameGraphPass*>::iterator;

        auto addEdge = [](FrameGraphPass* parent, FrameGraphPass* child)
        {
            if (parent != child)
            {
                if (!child->m_parents.Contains(parent))
                {
                    child->m_parents.Append(parent);
                    parent->m_childrens.Append(child);
                }
            }
        };

        for (FrameGraphPass& current: m_passes)
        {
            for (FrameGraphTextureHandle read: current.m_textureReads)
            {
                LastProducerIterator it = lastTextureProducer.Find(read.GetIndex());
                if (it != lastTextureProducer.end())
                {
                    FrameGraphPass* producer = it->Value;
                    addEdge(producer, &current);
                }
            }

            for (FrameGraphBufferHandle read: current.m_bufferReads)
            {
                LastProducerIterator it = lastBufferProducer.Find(read.GetIndex());
                if (it != lastBufferProducer.end())
                {
                    addEdge(it->Value, &current);
                }
            }

            for (FrameGraphTextureHandle write: current.m_textureWrites)
            {
                lastTextureProducer[write.GetIndex()] = &current;
            }

            if (current.m_depthStencil.HasValue())
            {
                lastTextureProducer[current.m_depthStencil->GetIndex()] = &current;
            }

            for (FrameGraphBufferHandle write: current.m_bufferWrites)
            {
                lastBufferProducer[write.GetIndex()] = &current;
            }
        }
    }

    void FrameGraph::TopoligicalSort()
    {
        HashMap<FrameGraphPass*, size_t> parentsCount(m_passes.GetSize());

        std::deque<size_t> deque;

        for (size_t i = 0; i < m_passes.GetSize(); i++)
        {
            FrameGraphPass& pass = m_passes[i];
            size_t count = pass.m_parents.GetSize();

            parentsCount[&pass] = count;
            if (count == 0)
            {
                deque.push_back(i);
            }
        }

        while (!deque.empty())
        {
            size_t passIndex = deque.front();
            FrameGraphPass& pass = m_passes[passIndex];
            deque.pop_front();

            m_sortedPasses.Append(passIndex);

            for (FrameGraphPass* child: pass.m_childrens)
            {
                parentsCount[child]--;
                if (parentsCount[child] == 0)
                {
                    deque.push_back(child->m_index);
                }
            }
        }
    }

    void FrameGraph::ComputeResourceLifetimes()
    {
        auto touch = [&](const FrameGraphTextureHandle& handle, size_t order)
        {
            FrameGraphTextureResource& resource = m_textures[handle.GetIndex()];
            resource.Lifetime.FirstUse = Math::Min(resource.Lifetime.FirstUse, order);
            resource.Lifetime.LastUse = Math::Max(resource.Lifetime.LastUse, order);
        };

        for (size_t orderPassIndex: m_sortedPasses)
        {
            FrameGraphPass& pass = m_passes[orderPassIndex];
            pass.m_order = orderPassIndex;

            for (const FrameGraphTextureHandle& handle: pass.m_textureReads)
            {
                touch(handle, orderPassIndex);
            }

            for (const FrameGraphTextureHandle& handle: pass.m_textureWrites)
            {
                touch(handle, orderPassIndex);
            }

            if (pass.m_depthStencil.HasValue())
            {
                touch(*pass.m_depthStencil, orderPassIndex);
            }
        }
    }

    void FrameGraph::ComputeRenderingInfoPasses()
    {
        for (size_t passIndex: m_sortedPasses)
        {
            FrameGraphPass& pass = m_passes[passIndex];
            if (pass.IsGraphicsStage())
            {
                m_passRenderingInfos[passIndex] = ComputeRenderingInfoPass(pass);
            }
        }
    }

    RHIGraphicsPipelineRenderingInfo FrameGraph::ComputeRenderingInfoPass(const FrameGraphPass& pass) const
    {
        RHIGraphicsPipelineRenderingInfo info = {};
        info.ColorAttachmentFormats.Reserve(pass.m_textureWrites.GetSize());

        for (FrameGraphTextureHandle handle: pass.m_textureWrites)
        {
            const FrameGraphTextureResource& resource = m_textures[handle.GetIndex()];
            info.ColorAttachmentFormats.Append(resource.Info.Format);
        }

        if (pass.m_depthStencil.HasValue())
        {
            FrameGraphTextureHandle handle = pass.m_depthStencil.Unwrap();
            const FrameGraphTextureResource& resource = m_textures[handle.GetIndex()];
            info.DepthAttachmentFormat = resource.Info.Format;

            if (RHIFormatIsDepthStencil(resource.Info.Format))
            {
                info.StencilAttachmentFormat = resource.Info.Format;
            }
        }

        return info;
    }

    FrameGraph::ResolvedStoreLoadResult FrameGraph::ResolveStoreLoadOp(FrameGraphPass& pass, FrameGraphTextureHandle handle)
    {
        FrameGraphTextureResource& resource = m_textures[handle.GetIndex()];
        FrameGraphResourceLifetime& lifetime = resource.Lifetime;

        bool isFirstUsage = lifetime.FirstUse == pass.GetOrder();
        bool isLastUsage = lifetime.LastUse == pass.GetOrder();
        bool isFrameGraphOutputResource = IsOutputResource(handle);

        pass.m_isFrameGraphOutput = pass.m_isFrameGraphOutput || isFrameGraphOutputResource;

        // TODO: Handle the case with LOAD_OP_DONTCARE.
        RHIAttachmentLoadOp loadOp = isFirstUsage ? RHIAttachmentLoadOp::Clear : RHIAttachmentLoadOp::Load;
        RHIAttachmentStoreOp outStoreOp = RHIAttachmentStoreOp::Store;

        return {loadOp, outStoreOp};
    }

    RHIBeginRenderingInfo FrameGraph::BuildRenderingInfo(FrameGraphPass& pass)
    {
        RHIBeginRenderingInfo info = {};
        info.ColorAttachments.Reserve(pass.m_textureWrites.GetSize());

        uint32_t width = 0;
        uint32_t height = 0;

        auto updateSize = [&](const FrameGraphTextureResource& resource)
        {
            if (width == 0 && height == 0)
            {
                width = resource.Info.Width;
                height = resource.Info.Height;
            }
            else
            {
                WL_CHECK_MSG(width == resource.Info.Width && height == resource.Info.Height, "All attachments must have the same size.");
            }
        };

        for (FrameGraphTextureHandle handle: pass.m_textureWrites)
        {
            FrameGraphTextureResource& resource = m_textures[handle.GetIndex()];

            RHIRenderingAttachmentInfo attachment = {};

            attachment.TextureView = ResolvePhysicalTexture(handle).View;

            auto [loadOp, storeOp] = ResolveStoreLoadOp(pass, handle);

            attachment.TextureLayout = RHITextureLayout::ColorAttachment;
            attachment.LoadOp = loadOp;
            attachment.StoreOp = storeOp;
            // TODO:
            // depthAttachment.ClearValue = resource.Description.ClearValue;

            info.ColorAttachments.Append(attachment);

            updateSize(resource);
        }

        if (pass.m_depthStencil.HasValue())
        {
            FrameGraphTextureHandle handle = pass.m_depthStencil.Unwrap();
            FrameGraphTextureResource& resource = m_textures[handle.GetIndex()];
            RHITextureView* textureView = ResolvePhysicalTexture(handle).View;

            auto [loadOp, storeOp] = ResolveStoreLoadOp(pass, handle);

            RHIRenderingAttachmentInfo depthAttachment = {};
            depthAttachment.TextureView = textureView;
            depthAttachment.TextureLayout = RHITextureLayout::DepthStencilAttachment;

            depthAttachment.LoadOp = loadOp;
            depthAttachment.StoreOp = storeOp;
            // TODO:
            // depthAttachment.ClearValue = resource.Description.ClearValue;

            info.DepthAttachment = depthAttachment;

            if (RHIFormatIsDepthStencil(resource.Info.Format))
            {
                RHIRenderingAttachmentInfo stencilAttachment = {};
                stencilAttachment.TextureView = textureView;
                stencilAttachment.TextureLayout = RHITextureLayout::DepthStencilAttachment;
                stencilAttachment.LoadOp = loadOp;
                stencilAttachment.StoreOp = storeOp;
                // TODO: stencilAttachment.ClearValue = resource.Description.ClearValue;

                info.StencilAttachment = stencilAttachment;
            }

            updateSize(resource);
        }

        info.RenderArea.Height = height;
        info.RenderArea.Width = width;
        info.RenderArea.X = 0.0f;
        info.RenderArea.Y = 0.0f;

        return info;
    }

    FrameGraphPhysicalTexture& FrameGraph::ResolvePhysicalTexture(const FrameGraphTextureHandle& handle)
    {
        FrameGraphTextureResource& resource = m_textures[handle.GetIndex()];
        if (resource.IsTransient)
        {
            return m_texturePool.GetResource(resource.PooledResource).PhysicalTexture;
        }

        return resource.PersistantResource;
    }

    void FrameGraph::BuildBarriers()
    {

        for (size_t passIndex: m_sortedPasses)
        {
            FrameGraphPass& pass = m_passes[passIndex];

            auto AddBarrier = [&](FrameGraphTextureHandle handle, RHITextureLayout layoutNeeded)
            {
                FrameGraphTextureResource& resource = m_textures[handle.GetIndex()];

                if (resource.CurrentLayout != layoutNeeded)
                {
                    FrameGraphTextureBarrier barrier = {};
                    barrier.Handle = handle;
                    barrier.OldLayout = resource.CurrentLayout;
                    barrier.NewLayout = layoutNeeded;
                    pass.m_barriers.Append(barrier);
                    resource.CurrentLayout = layoutNeeded;
                }
            };

            for (size_t i = 0; i < pass.m_textureReads.GetSize(); i++)
            {
                FrameGraphTextureHandle handle = pass.m_textureReads[i];
                RHITextureLayout layoutNeeded = pass.m_textureReadStates[handle];
                AddBarrier(handle, layoutNeeded);
            }

            for (size_t i = 0; i < pass.m_textureWrites.GetSize(); i++)
            {
                FrameGraphTextureHandle handle = pass.m_textureWrites[i];
                RHITextureLayout layoutNeeded = pass.m_textureWriteStates[handle];
                AddBarrier(handle, layoutNeeded);
            }

            if (pass.m_depthStencil)
            {
                FrameGraphTextureHandle handle = *pass.m_depthStencil;
                RHITextureLayout layoutNeeded = pass.m_textureWriteStates[handle];
                FrameGraphTextureResource& resource = m_textures[handle.GetIndex()];
                RHITextureLayout oldLayout = resource.CurrentLayout;
                AddBarrier(handle, layoutNeeded);
                // FIXME: This is a hack to fix the issue of depth stencil attachment being used as undefined layout.
                resource.CurrentLayout = oldLayout;
            }
        }
    }

    void FrameGraph::AllocatePhysicalPassResources(FrameGraphPass& pass)
    {
        auto allocate = [&](Array<FrameGraphTextureHandle>& textures)
        {
            for (FrameGraphTextureHandle handle: textures)
            {
                FrameGraphTextureResource& resource = m_textures[handle.GetIndex()];
                if (resource.Lifetime.FirstUse == pass.GetOrder())
                {
                    AllocatePhysicalResource(resource);
                }
            }
        };

        allocate(pass.m_textureReads);
        allocate(pass.m_textureWrites);

        if (pass.m_depthStencil)
        {
            FrameGraphTextureHandle handle = *pass.m_depthStencil;
            FrameGraphTextureResource& resource = m_textures[handle.GetIndex()];
            AllocatePhysicalResource(resource);
        }
    }

    void FrameGraph::AllocatePhysicalResource(FrameGraphTextureResource& resource)
    {
        WL_RETURN_WHEN(resource.IsAllocated || !resource.IsTransient);

        if (resource.Info.SizeClass == SizeClass::Swapchain)
        {
            resource.Info.Width = m_frameContext->GetSwapchain()->GetWidth();
            resource.Info.Height = m_frameContext->GetSwapchain()->GetHeight();
        }

        FrameGraphPhysicalTextureKey key = FrameGraphPhysicalTextureKey::Create(resource);
        PooledPhysicalTextureHandle pooledResource = m_texturePool.Obtain(key);

        resource.IsAllocated = true;
        resource.PooledResource = pooledResource;
    }

    void FrameGraph::DeallocatePhysicalPassResources(FrameGraphPass& pass)
    {
        auto deallocate = [&](Array<FrameGraphTextureHandle>& textures)
        {
            for (FrameGraphTextureHandle handle: textures)
            {
                FrameGraphTextureResource& resource = m_textures[handle.GetIndex()];
                if (resource.Lifetime.LastUse == pass.GetOrder())
                {
                    DeallocatePhysicalResource(resource);
                }
            }
        };

        deallocate(pass.m_textureReads);
        deallocate(pass.m_textureWrites);

        if (pass.m_depthStencil)
        {
            FrameGraphTextureHandle handle = *pass.m_depthStencil;
            FrameGraphTextureResource& resource = m_textures[handle.GetIndex()];
            DeallocatePhysicalResource(resource);
        }
    }

    void FrameGraph::DeallocatePhysicalResource(FrameGraphTextureResource& resource)
    {
        WL_RETURN_WHEN(!resource.IsAllocated || !resource.IsTransient);

        FrameGraphPhysicalTextureKey key = FrameGraphPhysicalTextureKey::Create(resource);

        m_texturePool.Release(key, resource.PooledResource);
        resource.IsAllocated = false;
    }

    void FrameGraph::DestroyPasses()
    {
        m_passNames.Clear();
        m_passes.Clear();
        m_sortedPasses.Clear();
        m_passRenderingInfos.Clear();
    }

    RHITextureLayoutTransition FrameGraph::BarrierToRHITransition(const FrameGraphTextureBarrier& barrier)
    {
        FrameGraphTextureResource& resource = m_textures[barrier.Handle.GetIndex()];
        WL_CHECK(resource.IsTransient);

        return RHITextureLayoutTransition(ResolvePhysicalTexture(barrier.Handle).Texture,
                                          barrier.OldLayout,
                                          barrier.NewLayout);
    }

    bool FrameGraph::IsOutputResource(FrameGraphTextureHandle handle)
    {
        // FIXME: Currently, only support swapchain resource as output.
        return m_outputs.Contains(handle);
    }

}// namespace Wl
