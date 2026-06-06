#include "Waterlily/Renderer/FrameGraph/FrameGraph.hpp"
#include "Waterlily/Core/Containers/HashMap.hpp"
#include "Waterlily/Core/Containers/Option.hpp"
#include "Waterlily/Core/Defines.hpp"
#include "Waterlily/Core/Math/Math.hpp"
#include "Waterlily/Core/Memory/SharedPtr.hpp"
#include "Waterlily/RHI/CommandBuffer.hpp"
#include "Waterlily/RHI/Device.hpp"
#include "Waterlily/RHI/Framebuffer.hpp"
#include "Waterlily/RHI/RHIForwards.hpp"
#include "Waterlily/RHI/RenderPass.hpp"
#include "Waterlily/RHI/Texture.hpp"
#include "Waterlily/RHI/Types.hpp"
#include "Waterlily/Renderer/FrameContext.hpp"
#include "Waterlily/Renderer/FrameGraph/FrameGraphCache.hpp"
#include "Waterlily/Renderer/FrameGraph/FrameGraphPass.hpp"
#include "Waterlily/Renderer/FrameGraph/FrameGraphPassBuilder.hpp"
#include "Waterlily/Renderer/FrameGraph/FrameGraphResource.hpp"

#include <deque>
#include <utility>

namespace Wl
{

    FrameGraph::FrameGraph(const SharedPtr<RHIDevice>& device, const SharedPtr<FrameContext>& frameContext)
        : m_device(device)
        , m_frameContext(frameContext)
        , m_framebufferCache(device)
        , m_renderPassCache(device)
        , m_texturePool(device)
        , m_textureCache(m_texturePool, m_device)
    {
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
                .PhysicalTexture = {},
                .IsTransient = true,
        });

        return index;
    }

    FrameGraphTextureHandle FrameGraph::ImportTexture(RHITexture* texture, RHITextureView* view)
    {
        FrameGraphTextureHandle index = m_textures.GetSize();
        m_textures.Append(FrameGraphResource::FromRHI(texture, view));
        return index;
    }

    FrameGraphBufferHandle FrameGraph::ImportBuffer(RHIBuffer* buffer, size_t range, size_t offset)
    {
        FrameGraphBufferHandle index = m_textures.GetSize();
        m_buffers.Append(FrameGraphResource::FromRHI(buffer, range, offset));
        return index;
    }

    void FrameGraph::UpdateImportedTexture(FrameGraphTextureHandle handle, RHITexture* texture, RHITextureView* view)
    {
        FrameGraphTextureResource resource = FrameGraphResource::FromRHI(texture, view);
        m_textures[handle.GetHandle()] = resource;
    }

    void FrameGraph::UpdateImportedBuffer(FrameGraphBufferHandle handle, RHIBuffer* buffer, size_t size, size_t offset)
    {
        FrameGraphBufferResource resource = FrameGraphResource::FromRHI(buffer, size, offset);
        m_buffers[handle.GetHandle()] = resource;
    }

    void FrameGraph::AddOutput(FrameGraphTextureHandle& handle)
    {
        m_outputs.Add(handle);
        m_textures[handle.GetHandle()].IsTransient = false;
    }

    FrameGraphPass& FrameGraph::AddPass(const StringID& name)
    {
        WL_CHECK_MSG(!m_passNames.Contains(name), Wl::Format("A pass named %s already exists in FrameGraph.", name.GetText().data()));
        m_passes.Emplace(name);
        m_passNames.Put(name, m_passes.GetSize() - 1);
        return m_passes.Back();
    }

    FrameGraphPass& FrameGraph::GetPass(const StringID& name)
    {
        WL_CHECK_MSG(m_passNames.Contains(name), Wl::Format("No pass named %s found in FrameGraph!", name.GetText().data()));
        size_t index = m_passNames[name];
        return m_passes[index];
    }

    RHIRenderPass* FrameGraph::GetRenderPass(const FrameGraphPass& pass)
    {
        return GetRenderPass(pass.GetName());
    }

    RHIRenderPass* FrameGraph::GetRenderPass(const StringID& name)
    {
        return m_renderPassCache.GetRenderPass(name);
    }

    void FrameGraph::BeginFrame()
    {
        m_passes.Clear();
        m_passNames.Clear();
        m_textures.Clear();
        m_textureCache.Clear();
        m_buffers.Clear();
        m_outputs.Clear();
        m_sortedPasses.Clear();
    }

    void FrameGraph::EndFrame()
    {
    }

    void FrameGraph::Compile()
    {
        WL_CHECK_MSG(m_outputs.GetSize() > 0, "FrameGraph must have at least one output.");

        for (size_t i = 0; i < m_passes.GetSize(); i++)
        {
            FrameGraphPass& pass = m_passes[i];
            pass.m_index = i;
     
            FrameGraphPassSetupContext context;
            context.FrameGraph = this;
            context.FrameContext = m_frameContext;
            context.Device = m_device;

            FrameGraphPassBuilder builder(pass, *this);

            pass.Setup(context, builder);
        }

        BuildDependencies();
        TopoligicalSort();
        WL_CHECK_MSG(m_sortedPasses.GetSize() == m_passes.GetSize(), "The Frame Graph given is not a DAG.");

        ComputeResourceLifetimes();

        BuildPasses();
        BuildBarriers();
    }

    void FrameGraph::Execute(RHICommandBuffer* commandBuffer)
    {
        for (size_t passIndex: m_sortedPasses)
        {
            FrameGraphPass& pass = m_passes[passIndex];

            AllocatePhysicalPassResources(passIndex);

            for (FrameGraphTextureBarrier& barrier: pass.m_barriers)
            {
                FrameGraphTextureResource& resource = m_textures[barrier.Handle.GetHandle()];
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
            context.Framebuffer = BuildFramebuffer(pass);
            context.RenderPass = GetRenderPass(pass);

            pass.Execute(context);

            DeallocatePhysicalPassResources(passIndex);
        }
    }

    void FrameGraph::Dispose()
    {
        m_device->WaitIdle();
        m_texturePool.Destroy();
        m_textureCache.Clear();
        DestroyPasses();
    }

    void FrameGraph::BuildDependencies()
    {
        HashMap<uint32_t, FrameGraphPass*> last_texture_producer;
        HashMap<uint32_t, FrameGraphPass*> last_buffer_producer;

        using LastProducerIterator = HashMap<uint32_t, FrameGraphPass*>::iterator;

        auto add_edge = [](FrameGraphPass* parent, FrameGraphPass* child)
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
                LastProducerIterator it = last_texture_producer.Find(read.GetHandle());
                if (it != last_texture_producer.end())
                {
                    FrameGraphPass* producer = (*it).Value;
                    add_edge(producer, &current);
                }
            }

            for (FrameGraphBufferHandle read: current.m_bufferReads)
            {
                LastProducerIterator it = last_buffer_producer.Find(read.GetHandle());
                if (it != last_buffer_producer.end())
                {
                    add_edge((*it).Value, &current);
                }
            }

            for (FrameGraphTextureHandle write: current.m_textureWrites)
            {
                last_texture_producer[write.GetHandle()] = &current;
            }

            if (current.m_depthStencil.HasValue())
            {
                last_texture_producer[current.m_depthStencil->GetHandle()] = &current;
            }

            for (FrameGraphBufferHandle write: current.m_bufferWrites)
            {
                last_buffer_producer[write.GetHandle()] = &current;
            }
        }
    }

    void FrameGraph::TopoligicalSort()
    {
        HashMap<FrameGraphPass*, size_t> parents_count(m_passes.GetSize());

        std::deque<size_t> deque;

        for (size_t i = 0; i < m_passes.GetSize(); i++)
        {
            FrameGraphPass& pass = m_passes[i];
            size_t count = pass.m_parents.GetSize();

            parents_count[&pass] = count;
            if (count == 0)
            {
                deque.push_back(i);
            }
        }

        while (!deque.empty())
        {
            size_t pass_index = deque.front();
            FrameGraphPass& pass = m_passes[pass_index];
            deque.pop_front();

            m_sortedPasses.Append(pass_index);

            for (FrameGraphPass* child: pass.m_childrens)
            {
                parents_count[child]--;
                if (parents_count[child] == 0)
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
            FrameGraphTextureResource& resource = m_textures[handle.GetHandle()];
            resource.Lifetime.FirstUse = Math::Min(resource.Lifetime.FirstUse, order);
            resource.Lifetime.LastUse = Math::Max(resource.Lifetime.LastUse, order);
        };

        for (size_t order = 0; order < m_sortedPasses.GetSize(); order++)
        {
            FrameGraphPass& pass = m_passes[m_sortedPasses[order]];
            pass.m_order = order;

            for (const FrameGraphTextureHandle& handle: pass.m_textureReads)
            {
                touch(handle, order);
            }

            for (const FrameGraphTextureHandle& handle: pass.m_textureWrites)
            {
                touch(handle, order);
            }

            if (pass.m_depthStencil.HasValue())
            {
                touch(*pass.m_depthStencil, order);
            }
        }
    }

    void FrameGraph::BuildPasses()
    {
        using ResolveStoreLoadResultType = std::pair<RHIAttachmentLoadOp, RHIAttachmentStoreOp>;
        using ResolveLayoutResultType = std::pair<RHITextureLayout, RHITextureLayout>;

        auto resolveStoreLoadOp = [&](size_t passIndex, FrameGraphTextureHandle handle) -> ResolveStoreLoadResultType
        {
            FrameGraphPass& pass = m_passes[passIndex];
            FrameGraphTextureResource& resource = m_textures[handle.GetHandle()];
            FrameGraphResourceLifetime& lifetime = resource.Lifetime;

            bool isFirstUsage = lifetime.FirstUse == passIndex;
            bool isLastUsage = lifetime.LastUse == passIndex;
            bool isFrameGraphOutputResource = IsOutputResource(handle);

            pass.m_isFrameGraphOutput = pass.m_isFrameGraphOutput || isFrameGraphOutputResource;

            // TODO: Handle the case with LOAD_OP_DONTCARE.
            RHIAttachmentLoadOp loadOp =
                    isFirstUsage ? RHIAttachmentLoadOp::Clear : RHIAttachmentLoadOp::Load;

            // TODO: Handle the case with external resource
            RHIAttachmentStoreOp outStoreOp = RHIAttachmentStoreOp::Store;
            if (isLastUsage)
            {
                outStoreOp = RHIAttachmentStoreOp::DontCare;
                if (!resource.IsTransient || isFrameGraphOutputResource)
                {
                    outStoreOp = RHIAttachmentStoreOp::Store;
                }
            }

            return {loadOp, outStoreOp};
        };

        auto resolveLayouts = [&](size_t passIndex, FrameGraphTextureHandle handle) -> ResolveLayoutResultType
        {
            FrameGraphPass& pass = m_passes[passIndex];
            FrameGraphTextureResource& resource = m_textures[handle.GetHandle()];
            FrameGraphResourceLifetime& lifetime = resource.Lifetime;

            bool isFirstUsage = lifetime.FirstUse == passIndex;
            bool isLastUsage = lifetime.LastUse == passIndex;
            bool isResourceFrameGraphOutput = IsOutputResource(handle);
            bool isRead = pass.m_textureReads.Contains(handle);
            bool isWrite = pass.m_textureWrites.Contains(handle);

            RHITextureLayout initialLayout = RHITextureLayout::Undefined;
            RHITextureLayout finalLayout = RHITextureLayout::Undefined;

            if (isFirstUsage)
            {
                initialLayout = RHITextureLayout::Undefined;
            }
            else
            {
                initialLayout = resource.InitialLayout;
            }

            finalLayout = pass.m_textureWriteStates[handle.GetHandle()];
            if (isLastUsage && isResourceFrameGraphOutput)
            {
                finalLayout = RHITextureLayout::Present;
            }

            return {initialLayout, finalLayout};
        };

        for (size_t passIndex: m_sortedPasses)
        {
            FrameGraphPass& pass = m_passes[passIndex];

            if (m_renderPassCache.GetRenderPass(pass.GetName()))
            {
                continue;
            }

            RHIRenderPassDescription renderPassDescription = {};

            renderPassDescription.ColorAttachmentDecriptions.Reserve(pass.m_textureWrites.GetSize());
            for (FrameGraphTextureHandle& handle: pass.m_textureWrites)
            {
                FrameGraphTextureResource& resource = m_textures[handle.GetHandle()];

                auto [loadOp, storeOp] = resolveStoreLoadOp(passIndex, handle);
                auto [initialLayout, finalLayout] = resolveLayouts(passIndex, handle);

                RHIColorAttachmentDescription colorAttachment = {};
                colorAttachment.Format = resource.Info.Format;
                colorAttachment.LoadOp = loadOp;
                colorAttachment.StoreOp = storeOp;
                colorAttachment.InitialLayout = initialLayout;
                colorAttachment.FinalLayout = finalLayout;

                renderPassDescription.ColorAttachmentDecriptions.Append(colorAttachment);
            }

            if (pass.m_depthStencil.HasValue())
            {
                FrameGraphTextureHandle& handle = *pass.m_depthStencil;
                FrameGraphTextureResource& resource = m_textures[handle.GetHandle()];

                auto [loadOp, storeOp] = resolveStoreLoadOp(passIndex, handle);

                RHIDepthAttachmentDescription depthStencilAttachmentDescription = {};
                depthStencilAttachmentDescription.Format = resource.Info.Format;
                depthStencilAttachmentDescription.LoadOp = RHIAttachmentLoadOp::Clear;
                depthStencilAttachmentDescription.StoreOp = storeOp;

                renderPassDescription.DepthAttachmentDescription = depthStencilAttachmentDescription;
            }

            m_renderPassCache.Create(pass.GetName(), renderPassDescription);
        }
    }
    
    void FrameGraph::BuildBarriers()
    {
        for (size_t passIndex: m_sortedPasses)
        {
            FrameGraphPass& pass = m_passes[passIndex];

            for (size_t i = 0; i < pass.m_textureReads.GetSize(); i++)
            {
                FrameGraphTextureHandle handle = pass.m_textureReads[i];
                RHITextureLayout layoutNeeded = pass.m_textureReadStates[handle];

                FrameGraphTextureResource& resource = m_textures[handle.GetHandle()];

                FrameGraphTextureBarrier barrier;
                barrier.Handle = handle;
                barrier.OldLayout = resource.FinalLayout;
                barrier.NewLayout = layoutNeeded;

                resource.InitialLayout = resource.FinalLayout;
                resource.FinalLayout = layoutNeeded;

                pass.m_barriers.Append(barrier);
            }

            for (size_t i = 0; i < pass.m_textureWrites.GetSize(); i++)
            {
                FrameGraphTextureHandle handle = pass.m_textureWrites[i];
                RHITextureLayout layoutNeeded = pass.m_textureWriteStates[handle];

                FrameGraphTextureResource& resource = m_textures[handle.GetHandle()];

                FrameGraphTextureBarrier barrier;
                barrier.Handle = handle;
                barrier.OldLayout = resource.FinalLayout;
                barrier.NewLayout = layoutNeeded;

                pass.m_barriers.Append(barrier);
            }
        }
    }

    void FrameGraph::AllocatePhysicalPassResources(size_t passIndex)
    {
        auto allocate = [&](Array<FrameGraphTextureHandle>& textures)
        {
            for (FrameGraphTextureHandle handle: textures)
            {
                FrameGraphTextureResource& resource = m_textures[handle.GetHandle()];

                if (!resource.IsTransient || IsOutputResource(handle))
                {
                    continue;
                }

                if (resource.Lifetime.FirstUse == passIndex)
                {
                    AllocatePhysicalResource(resource);
                }
            }
        };

        FrameGraphPass& pass = m_passes[passIndex];
        allocate(pass.m_textureReads);
        allocate(pass.m_textureWrites);

        if (pass.m_depthStencil)
        {
            FrameGraphTextureHandle handle = pass.m_depthStencil->GetHandle();
            FrameGraphTextureResource& resource = m_textures[handle.GetHandle()];
            if (resource.IsTransient)
            {
                AllocatePhysicalResource(resource);
            }
        }
    }

    void FrameGraph::DeallocatePhysicalPassResources(size_t passIndex)
    {
        auto deallocate = [&](Array<FrameGraphTextureHandle>& textures)
        {
            for (FrameGraphTextureHandle handle: textures)
            {
                FrameGraphTextureResource& resource = m_textures[handle.GetHandle()];

                if (!resource.IsTransient || IsOutputResource(handle))
                {
                    continue;
                }

                if (resource.Lifetime.LastUse == passIndex)
                {
                    DeallocatePhysicalResource(resource);
                }
            }
        };

        FrameGraphPass& pass = m_passes[passIndex];
        deallocate(pass.m_textureReads);
        deallocate(pass.m_textureWrites);

        if (pass.m_depthStencil)
        {
            FrameGraphTextureHandle handle = pass.m_depthStencil->GetHandle();
            FrameGraphTextureResource& resource = m_textures[handle];
            if (resource.IsTransient)
            {
                DeallocatePhysicalResource(resource);
            }
        }
    }

    void FrameGraph::AllocatePhysicalResource(FrameGraphTextureResource& resource)
    {
        if (resource.IsAllocated)
        {
            return;
        }

        if (resource.Info.SizeClass == SizeClass::Swapchain)
        {
            resource.Info.Width = m_frameContext->GetSwapchain()->GetWidth();
            resource.Info.Height = m_frameContext->GetSwapchain()->GetHeight();
        }

        FrameGraphPhysicalTextureKey key = {
                .Format = resource.Info.Format,
                .Usage = resource.Usage,
                .Width = resource.Info.Width,
                .Height = resource.Info.Height,
        };

        FrameGraphPhysicalTexturePool::Handle pooledPhysicalTextureHandle = m_texturePool.Obtain(key);

        resource.PoolHandle = pooledPhysicalTextureHandle;
        resource.IsAllocated = true;
        resource.PhysicalTexture = m_texturePool.GetResource(pooledPhysicalTextureHandle);
    }

    void FrameGraph::DeallocatePhysicalResource(FrameGraphTextureResource& resource)
    {
        if (!resource.IsAllocated)
        {
            return;
        }

        FrameGraphPhysicalTextureKey key = {
                .Format = resource.Info.Format,
                .Usage = resource.Usage,
                .Width = resource.Info.Width,
                .Height = resource.Info.Height,
        };

        m_texturePool.Release(resource.PoolHandle);
        resource.IsAllocated = false;
    }

    RHIFramebuffer* FrameGraph::BuildFramebuffer(FrameGraphPass& pass)
    {
        RHIFramebufferDescription description = {};
        description.RenderPass = m_renderPassCache.GetRenderPass(pass.GetName());
        description.Width = 0;
        description.Height = 0;
        description.Layers = 1;

        auto updateFramebufferSize = [&](uint32_t width, uint32_t height)
        {
            if (description.Width == 0 && description.Height == 0)
            {
                description.Width = width;
                description.Height = height;
            }
            else
            {
                WL_CHECK_MSG(description.Width == width && description.Height == height, "All attachments in a framebuffer must have the same size!");
            }
        };

        size_t depthCost = (pass.m_depthStencil.HasValue() ? 1 : 0);
        description.Attachments.Reserve(pass.m_textureWrites.GetSize() + depthCost);

        for (FrameGraphTextureHandle handle: pass.m_textureWrites)
        {
            FrameGraphTextureResource& resource = m_textures[handle.GetHandle()];

            if (IsOutputResource(handle))
            {
                RHITextureView* swapchainTextureView = m_frameContext->GetSwapchain()->GetCurrentTextureView();
                resource.Info.Width = m_frameContext->GetSwapchain()->GetWidth();
                resource.Info.Height = m_frameContext->GetSwapchain()->GetHeight();
                description.Attachments.Append(swapchainTextureView);
            }
            else
            {
                description.Attachments.Append(resource.PhysicalTexture.View);
            }

            updateFramebufferSize(resource.Info.Width, resource.Info.Height);
        }

        if (pass.m_depthStencil.HasValue())
        {
            FrameGraphTextureHandle handle = *pass.m_depthStencil;
            FrameGraphTextureResource& resource = m_textures[handle.GetHandle()];
            description.Attachments.Append(resource.PhysicalTexture.View);
            updateFramebufferSize(resource.Info.Width, resource.Info.Height);
        }

        WL_CHECK_MSG(description.Width > 0 && description.Height > 0, "Framebuffer must have a valid size (> 0)");

        return m_framebufferCache.Obtain(description);
    }

    void FrameGraph::DestroyPasses()
    {
        m_passNames.Clear();
        m_passes.Clear();
        m_renderPassCache.Clear();
        m_sortedPasses.Clear();
    }

    RHITextureLayoutTransition FrameGraph::BarrierToRHITransition(const FrameGraphTextureBarrier& barrier)
    {
        FrameGraphTextureResource& resource = m_textures[barrier.Handle.GetHandle()];
        return RHITextureLayoutTransition(resource.PhysicalTexture.Texture,
                                          barrier.OldLayout,
                                          barrier.NewLayout);
    }

    bool FrameGraph::IsOutputResource(FrameGraphTextureHandle handle)
    {
        // FIXME: Currently, only support swapchain resource as output.
        return m_outputs.Contains(handle);
    }

}// namespace Wl
