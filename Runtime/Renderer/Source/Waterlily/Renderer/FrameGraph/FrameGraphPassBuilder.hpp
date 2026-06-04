#pragma once

#include "Waterlily/Renderer/FrameGraph/FrameGraph.hpp"
#include "Waterlily/Renderer/FrameGraph/FrameGraphPass.hpp"
#include "Waterlily/Renderer/FrameGraph/FrameGraphResource.hpp"
#include "Waterlily/Renderer/RendererExports.hpp"

namespace Wl
{

    class WL_RENDERER_API FrameGraphPassBuilder
    {
    public:
        void Read(FrameGraphTextureHandle handle);
        void Write(FrameGraphTextureHandle handle);
        void WriteStorage(FrameGraphTextureHandle handle);

        void ReadStorage(FrameGraphBufferHandle handle);
        void ReadUniform(FrameGraphBufferHandle handle);
        void WriteStorage(FrameGraphBufferHandle handle);

        void SetDepthStencil(FrameGraphTextureHandle handle);

        void SetStage(FrameGraphPassStage stage);

    public:
        FrameGraphPassBuilder(FrameGraphPass& pass, FrameGraph& framegraph)
            : m_pass(pass)
            , m_framegraph(framegraph)
        {
        }

    private:
        FrameGraphPass& m_pass;
        FrameGraph& m_framegraph;
    };

}// namespace Wl