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

        m_framegraph.GetTexture(handle).Usage |= RHITextureUsageFlags::Sampler;
    }

    void FrameGraphPassBuilder::Write(FrameGraphTextureHandle handle)
    {
        m_pass.m_textureWrites.Append(handle);
        m_pass.m_textureWriteStates[handle] = RHITextureLayout::ColorAttachment;

        m_framegraph.GetTexture(handle).Usage |= RHITextureUsageFlags::ColorAttachment;
    }

    void FrameGraphPassBuilder::WriteStorage(FrameGraphTextureHandle handle)
    {
        m_pass.m_textureWrites.Append(handle);
        m_pass.m_textureWriteStates[handle] = RHITextureLayout::General;

        m_framegraph.GetTexture(handle).Usage |= RHITextureUsageFlags::Storage;
    }

    void FrameGraphPassBuilder::ReadStorage(FrameGraphBufferHandle handle)
    {
        m_pass.m_bufferReads.Append(handle);
        m_framegraph.GetBuffer(handle).Usage |= RHIBufferUsageFlags::Storage;
    }

    void FrameGraphPassBuilder::ReadUniform(FrameGraphBufferHandle handle)
    {
        m_pass.m_bufferReads.Append(handle);
        m_framegraph.GetBuffer(handle).Usage |= RHIBufferUsageFlags::Uniform;
    }

    void FrameGraphPassBuilder::WriteStorage(FrameGraphBufferHandle handle)
    {
        m_pass.m_bufferWrites.Append(handle);
        m_framegraph.GetBuffer(handle).Usage |= RHIBufferUsageFlags::Storage;
    }

    void FrameGraphPassBuilder::SetDepthStencil(FrameGraphTextureHandle handle)
    {
        m_pass.m_depthStencil = handle;
        m_framegraph.GetTexture(handle).Usage |= RHITextureUsageFlags::DepthStencilAttachment;
    }

    void FrameGraphPassBuilder::SetStage(FrameGraphPassStage stage)
    {
        m_pass.m_stage = stage;
    }

}// namespace Wl
