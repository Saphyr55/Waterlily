#pragma once

#include "Waterlily/Core/Containers/HashMap.hpp"
#include "Waterlily/Core/IO/FileSystem.hpp"
#include "Waterlily/Core/Memory/SharedPtr.hpp"
#include "Waterlily/Core/String/StringID.hpp"
#include "Waterlily/RHI/Device.hpp"
#include "Waterlily/RHI/GraphicsPipeline.hpp"
#include "Waterlily/RHI/RenderPass.hpp"
#include "Waterlily/RHI/ShaderResource.hpp"
#include "Waterlily/RHI/ShaderResourceCache.hpp"
#include "Waterlily/Renderer/RendererExports.hpp"

namespace Wl
{

    struct GraphicsPipelineProperties
    {
        StringID VertexShaderPath;
        StringID FragmentShaderPath;
        RHIRenderPass* RenderPass = nullptr;
        Viewport Viewport = {};
        Rect2D Scissor = {};
        RHICullModeFlags CullMode = RHICullModeFlags::Back;
        HashMap<uint32_t, RHIShaderResourceGroupLayout*> SRGLayouts;
    };

    class WL_RENDERER_API PipelineManager
    {
    public:
        RHIGraphicsPipeline* GetPipeline(const StringID& name);
        RHIGraphicsPipeline* GetOrCreate(const StringID& name, GraphicsPipelineProperties& props);
        RHIGraphicsPipeline* Create(const StringID& name, GraphicsPipelineProperties& props);
        RHIGraphicsPipeline* Recreate(const StringID& name, GraphicsPipelineProperties& props);
        void Destroy(const StringID& name);

        HashMap<StringID, RHIGraphicsPipeline*>& GetPipelines()
        {
            return m_cache;
        }

        const HashMap<StringID, RHIGraphicsPipeline*>& GetPipelines() const
        {
            return m_cache;
        }

        void Destroy()
        {
            for (auto [name, pipeline]: GetPipelines())
            {
                Destroy(name);
            }
        }

    public:
        PipelineManager(SharedPtr<RHIDevice> device,
                        SharedPtr<RHIShaderResourceGroupLayoutCache> srgLayoutCache,
                        FileSystem& fileSystem)
            : m_device(device)
            , m_srgLayoutCache(srgLayoutCache)
            , m_fileSystem(fileSystem)
        {
        }

    private:
        RHIGraphicsPipeline* CreateInternal(GraphicsPipelineProperties& props);
        void DestroyInternal(RHIGraphicsPipeline* pipeline);

    private:
        SharedPtr<RHIDevice> m_device;
        SharedPtr<RHIShaderResourceGroupLayoutCache> m_srgLayoutCache;
        FileSystem& m_fileSystem;
        HashMap<StringID, RHIGraphicsPipeline*> m_cache;
    };

}// namespace Wl
