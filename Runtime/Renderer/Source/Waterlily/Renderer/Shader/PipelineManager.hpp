#pragma once

#include "Waterlily/Core/Containers/HashMap.hpp"
#include "Waterlily/Core/IO/FileSystem.hpp"
#include "Waterlily/Core/Memory/SharedPtr.hpp"
#include "Waterlily/Core/String/StringID.hpp"
#include "Waterlily/RHI/ComputePipeline.hpp"
#include "Waterlily/RHI/Device.hpp"
#include "Waterlily/RHI/GraphicsPipeline.hpp"
#include "Waterlily/RHI/RenderPass.hpp"
#include "Waterlily/RHI/ShaderResource.hpp"
#include "Waterlily/RHI/ShaderResourceCache.hpp"
#include "Waterlily/Renderer/RendererExports.hpp"
#include "Waterlily/Renderer/Shader/Shader.hpp"
#include "Waterlily/Renderer/Shader/ShaderParser.hpp"

namespace Wl
{

    struct Frame;

    struct GraphicsPipelineState
    {
        RHIRenderPass* RenderPass = nullptr;
        Shader* VertexShader;
        Shader* FragmentShader;
        Viewport Viewport = {};
        Rect2D Scissor = {};
        RHICullModeFlags CullMode = RHICullModeFlags::Back;
        HashMap<uint32_t, RHIShaderResourceGroupLayout*> SRGLayouts;
    };

    struct ComputePipelineState
    {
        Shader* ComputeShader;
        HashMap<uint32_t, RHIShaderResourceGroupLayout*> SRGLayouts;
    };

    class WL_RENDERER_API PipelineManager
    {
    public:
        void CreateFrameSRGPool(ArrayView<Frame> frames);
        void ResetFrameSRGPool();

    public:
        // Graphics Pipeline
        RHIGraphicsPipeline* GetGraphicsPipeline(const StringID& name);
        RHIGraphicsPipeline* GetOrCreateGraphicsPipeline(const StringID& name, GraphicsPipelineState& state);

        RHIGraphicsPipeline* CreateGraphicsPipeline(const StringID& name, GraphicsPipelineState& state);
        RHIGraphicsPipeline* RecreateGraphicsPipeline(const StringID& name, GraphicsPipelineState& state);
        void DestroyGraphicsPipeline(const StringID& name);

        HashMap<StringID, RHIGraphicsPipeline*>& GetGraphicsPipelines()
        {
            return m_cache;
        }

        const HashMap<StringID, RHIGraphicsPipeline*>& GetGraphicsPipelines() const
        {
            return m_cache;
        }

    public:
        // Compute Pipeline
        RHIComputePipeline* GetComputePipeline(const StringID& name);
        RHIComputePipeline* GetOrCreateComputePipeline(const StringID& name, ComputePipelineState& state);

        RHIComputePipeline* CreateComputePipeline(const StringID& name, ComputePipelineState& state);
        RHIComputePipeline* RecreateComputePipeline(const StringID& name, ComputePipelineState& state);
        void DestroyComputePipeline(const StringID& name);

        HashMap<StringID, RHIComputePipeline*>& GetComputePipelines()
        {
            return m_computePipelineCache;
        }

        const HashMap<StringID, RHIComputePipeline*>& GetComputePipelines() const
        {
            return m_computePipelineCache;
        }

    public:
        void Destroy()
        {
            for (auto [name, pipeline]: GetGraphicsPipelines())
            {
                DestroyGraphicsPipeline(name);
            }

            for (auto [name, pipeline]: GetComputePipelines())
            {
                DestroyComputePipeline(name);
            }

            m_srgLayoutCache.Dispose();
        }

        const RHIShaderResourceGroupLayoutCache& GetSRGLayoutCache() const
        {
            return m_srgLayoutCache;
        }

        RHIShaderResourceGroupLayoutCache& GetSRGLayoutCache()
        {
            return m_srgLayoutCache;
        }

    public:
        PipelineManager(SharedPtr<RHIDevice> device,
                        FileSystem& fileSystem)
            : m_device(device)
            , m_srgLayoutCache(device)
            , m_fileSystem(fileSystem)
        {
        }

    private:
        RHIGraphicsPipeline* CreateInternalGraphicsPipeline(GraphicsPipelineState& state);
        void DestroyInternalGraphicsPipeline(RHIGraphicsPipeline* pipeline);
        RHIComputePipeline* CreateInternalComputePipeline(ComputePipelineState& state);
        void DestroyInternalComputePipeline(RHIComputePipeline* pipeline);

        void ReflectSRGLayouts(
                SPIRVPipelineReflection& outReflection,
                HashMap<uint32_t, RHIShaderResourceGroupLayout*>& outStateLayouts,
                Array<RHIShaderResourceGroupLayout*>& outLayouts);

    private:
        SharedPtr<RHIDevice> m_device;
        RHIShaderResourceGroupLayoutCache m_srgLayoutCache;
        FileSystem& m_fileSystem;
        HashMap<StringID, RHIGraphicsPipeline*> m_cache;
        HashMap<StringID, RHIComputePipeline*> m_computePipelineCache;
        bool m_isResetSRGPool = true;
    };

}// namespace Wl
