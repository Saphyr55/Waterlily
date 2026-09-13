#include "Waterlily/Renderer/FrameGraph/FrameGraphPassBuilder.hpp"
#include "Waterlily/RHI/Types.hpp"
#include "Waterlily/Renderer/FrameGraph/FrameGraphPass.hpp"
#include "Waterlily/Renderer/FrameGraph/FrameGraphResource.hpp"

namespace Wl
{

    void FrameGraphPassBuilder::Read(FrameGraphTextureHandle handle)
    {
        m_pass.m_textureReads.Append(handle);
        m_pass.m_textureReadStates[handle] = RHITextureLayout::ShaderReadOnly;

        FrameGraphTextureResource& resource = m_framegraph.GetTexture(handle);
        resource.Usage |= RHITextureUsageFlags::Sampler;
    }

    void FrameGraphPassBuilder::ReadStorage(FrameGraphTextureHandle handle)
    {
        m_pass.m_textureReads.Append(handle);
        m_pass.m_textureReadStates[handle] = RHITextureLayout::General;

        FrameGraphTextureResource& resource = m_framegraph.GetTexture(handle);
        resource.Usage |= RHITextureUsageFlags::Storage;
    }

    void FrameGraphPassBuilder::Write(FrameGraphTextureHandle handle)
    {
        m_pass.m_textureWrites.Append(handle);
        m_pass.m_textureWriteStates[handle] = RHITextureLayout::ColorAttachment;

        FrameGraphTextureResource& resource = m_framegraph.GetTexture(handle);
        resource.Usage |= RHITextureUsageFlags::ColorAttachment;
    }

    void FrameGraphPassBuilder::WriteStorage(FrameGraphTextureHandle handle)
    {
        m_pass.m_textureWrites.Append(handle);
        m_pass.m_textureWriteStates[handle] = RHITextureLayout::General;

        FrameGraphTextureResource& resource = m_framegraph.GetTexture(handle);
        resource.Usage |= RHITextureUsageFlags::Storage;
    }

    void FrameGraphPassBuilder::ReadStorage(FrameGraphBufferHandle handle)
    {
        m_pass.m_bufferReads.Append(handle);

        FrameGraphBufferResource& resource = m_framegraph.GetBuffer(handle);
        resource.Usage |= RHIBufferUsageFlags::Storage;
    }

    void FrameGraphPassBuilder::ReadUniform(FrameGraphBufferHandle handle)
    {
        m_pass.m_bufferReads.Append(handle);

        FrameGraphBufferResource& resource = m_framegraph.GetBuffer(handle);
        resource.Usage |= RHIBufferUsageFlags::Uniform;
    }

    void FrameGraphPassBuilder::WriteStorage(FrameGraphBufferHandle handle)
    {
        m_pass.m_bufferWrites.Append(handle);

        FrameGraphBufferResource& resource = m_framegraph.GetBuffer(handle);
        resource.Usage |= RHIBufferUsageFlags::Storage;
    }

    void FrameGraphPassBuilder::SetDepthStencil(FrameGraphTextureHandle handle)
    {
        m_pass.m_depthStencil = handle;
        m_pass.m_textureWriteStates[handle] = RHITextureLayout::DepthStencilAttachment;

        FrameGraphTextureResource& resource = m_framegraph.GetTexture(handle);
        resource.Usage |= RHITextureUsageFlags::DepthStencilAttachment;
    }

    void FrameGraphPassBuilder::SetStage(FrameGraphPassStage stage)
    {
        m_pass.m_stage = stage;
    }

}// namespace Wl
