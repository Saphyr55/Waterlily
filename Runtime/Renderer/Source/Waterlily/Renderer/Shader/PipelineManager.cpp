#include "Waterlily/Renderer/Shader/PipelineManager.hpp"
#include "Waterlily/Core/Containers/Array.hpp"
#include "Waterlily/Core/Defines.hpp"
#include "Waterlily/Core/Logging/Trace.hpp"
#include "Waterlily/Core/String/StringID.hpp"
#include "Waterlily/RHI/ComputePipeline.hpp"
#include "Waterlily/RHI/GraphicsPipeline.hpp"
#include "Waterlily/RHI/ShaderResource.hpp"
#include "Waterlily/RHI/Types.hpp"
#include "Waterlily/Renderer/FrameContext.hpp"
#include "Waterlily/Renderer/Shader/ShaderParser.hpp"

namespace Wl
{

    void PipelineManager::CreateFrameSRGPool(ArrayView<Frame> frames)
    {
        if (!m_isResetSRGPool)
        {
            return;
        }

        for (Frame& frame: frames)
        {
            Array<RHIShaderResourceBinding> totalBindings;
            for (const RHIShaderResourceGroupLayout* srgLayout: GetSRGLayoutCache().GetResources())
            {
                totalBindings.AppendRange(srgLayout->GetBindings());
            }

            if (frame.SRGPool != nullptr)
            {
                m_device->DestroySRGPool(frame.SRGPool);
            }

            frame.SRGPool = m_device->CreateSRGPool(256, totalBindings);
        }

        m_isResetSRGPool = false;
    }

    void PipelineManager::ResetFrameSRGPool()
    {
        GetSRGLayoutCache().Dispose();
        m_isResetSRGPool = true;
    }

    RHIGraphicsPipeline* PipelineManager::CreateGraphicsPipeline(const StringID& name, GraphicsPipelineState& state)
    {
        WL_CHECK_MSG(!m_cache.Contains(name), "The pipeline named '%s' already exist.", name.GetText().data());

        RHIGraphicsPipeline* pipeline = CreateInternalGraphicsPipeline(state);
        m_cache[name] = pipeline;
        return pipeline;
    }

    RHIGraphicsPipeline* PipelineManager::GetOrCreateGraphicsPipeline(const StringID& name, GraphicsPipelineState& state)
    {
        if (RHIGraphicsPipeline* pipeline = GetGraphicsPipeline(name))
        {
            return pipeline;
        }
        return CreateGraphicsPipeline(name, state);
    }

    RHIGraphicsPipeline* PipelineManager::GetGraphicsPipeline(const StringID& name)
    {
        HashMap<StringID, RHIGraphicsPipeline*>::iterator it = m_cache.find(name);
        if (it == m_cache.end())
        {
            return nullptr;
        }
        return it->Value;
    }

    RHIGraphicsPipeline* PipelineManager::RecreateGraphicsPipeline(const StringID& name, GraphicsPipelineState& state)
    {
        DestroyGraphicsPipeline(name);
        return CreateGraphicsPipeline(name, state);
    }

    void PipelineManager::DestroyGraphicsPipeline(const StringID& name)
    {
        RHIGraphicsPipeline* pipeline = GetGraphicsPipeline(name);
        WL_CHECK_MSG(pipeline, "Pipeline \"%s\" not found.", name.GetText().GetData());
        m_cache.Remove(name);
        DestroyInternalGraphicsPipeline(pipeline);
    }

    RHIComputePipeline* PipelineManager::GetComputePipeline(const StringID& name)
    {
        HashMap<StringID, RHIComputePipeline*>::iterator it = m_computePipelineCache.find(name);
        if (it == m_computePipelineCache.end())
        {
            return nullptr;
        }
        return it->Value;
    }

    RHIComputePipeline* PipelineManager::GetOrCreateComputePipeline(const StringID& name, ComputePipelineState& state)
    {
        if (RHIComputePipeline* pipeline = GetComputePipeline(name))
        {
            return pipeline;
        }
        return CreateComputePipeline(name, state);
    }

    RHIComputePipeline* PipelineManager::CreateComputePipeline(const StringID& name, ComputePipelineState& state)
    {
        WL_CHECK_MSG(!m_cache.Contains(name), "The pipeline named '%s' already exist.", name.GetText().data());

        RHIComputePipeline* pipeline = CreateInternalComputePipeline(state);
        m_computePipelineCache[name] = pipeline;
        return pipeline;
    }

    RHIComputePipeline* PipelineManager::RecreateComputePipeline(const StringID& name, ComputePipelineState& state)
    {
        DestroyComputePipeline(name);
        return CreateComputePipeline(name, state);
    }

    void PipelineManager::DestroyComputePipeline(const StringID& name)
    {
        RHIComputePipeline* pipeline = GetComputePipeline(name);
        WL_CHECK_MSG(pipeline, "Pipeline \"%s\" not found.", name.GetText().GetData());
        m_cache.Remove(name);
        DestroyInternalComputePipeline(pipeline);
    }

    void PipelineManager::DestroyInternalGraphicsPipeline(RHIGraphicsPipeline* pipeline)
    {
        WL_CHECK(pipeline);
        m_device->DestroyGraphicsPipeline(pipeline);
    }

    void PipelineManager::DestroyInternalComputePipeline(RHIComputePipeline* pipeline)
    {
        WL_CHECK(pipeline);
        m_device->DestroyComputePipeline(pipeline);
    }

    RHIGraphicsPipeline* PipelineManager::CreateInternalGraphicsPipeline(GraphicsPipelineState& state)
    {
        WL_CHECK_MSG(state.RenderPass, "The GraphicsPipelineProperties.RenderPass is not nullable to create a graphics pipeline.");
        WL_CHECK_MSG(state.VertexShader, "The GraphicsPipelineProperties.VertexShader is not nullable to create a graphics pipeline.");
        WL_CHECK_MSG(state.FragmentShader, "The GraphicsPipelineProperties.FragmentShader is not nullable to create a graphics pipeline.");

        const SPIRVShader& vertexShader = state.VertexShader->GetSPIRVShader();
        const SPIRVShader& fragmentShader = state.FragmentShader->GetSPIRVShader();

        SPIRVPipelineReflection reflection;
        bool result = SPIRVPipelineReflector::Reflect(reflection, {vertexShader, fragmentShader});
        WL_RETURN_OBJECT_WHEN(!result, nullptr);

        Array<RHIShaderResourceGroupLayout*> srgLayouts;
        ReflectSRGLayouts(reflection, state.SRGLayouts, srgLayouts);

        Array<RHIVertexBindingDescription> bindings;
        Array<RHIVertexAttributeDescription> attributes;

        for (const SPIRVVertexInput& vertexInput: reflection.VertexInputs)
        {
            bindings.Emplace(vertexInput.Location, vertexInput.Stride, RHIVertexInputRate::Vertex);
            attributes.Emplace(vertexInput.Location, vertexInput.Location, vertexInput.Format);
        }

        RHIGraphicsPipelineDescriptionBuilder builder;
        builder.WithRenderPass(state.RenderPass)
                .WithVertexShader(vertexShader, reflection.EntryPointNames[RHIShaderStage::Vertex])
                .WithFragmentShader(fragmentShader, reflection.EntryPointNames[RHIShaderStage::Fragment])
                .WithVertexBindings(bindings, attributes)
                .WithViewport(state.Viewport, state.Scissor)
                .WithSRGLayout(srgLayouts)
                .WithCullMode(state.CullMode);

        const RHIGraphicsPipelineDescription& description = builder.Build();
        return m_device->CreateGraphicsPipeline(description);
    }

    RHIComputePipeline* PipelineManager::CreateInternalComputePipeline(ComputePipelineState& state)
    {
        WL_CHECK_MSG(state.ComputeShader, "The ComputePipelineState.ComputeShader is not nullable to create a graphics pipeline.");

        const SPIRVShader& computeShader = state.ComputeShader->GetSPIRVShader();

        SPIRVPipelineReflection reflection;
        bool result = SPIRVPipelineReflector::Reflect(reflection, {computeShader});
        WL_RETURN_OBJECT_WHEN(!result, nullptr);

        Array<RHIShaderResourceGroupLayout*> srgLayouts;
        ReflectSRGLayouts(reflection, state.SRGLayouts, srgLayouts);

        RHIComputePipelineDescription description = {};
        description.ComputeShaderInfo = RHIPipelineShaderStageCreateInfo {
                .Stage = RHIShaderStage::Compute,
                .Name = reflection.EntryPointNames[RHIShaderStage::Compute],
                .Shader = computeShader};
        description.SRGLayouts = srgLayouts;

        return m_device->CreateComputePipeline(description);
    }

    void PipelineManager::ReflectSRGLayouts(
            SPIRVPipelineReflection& reflection,
            HashMap<uint32_t, RHIShaderResourceGroupLayout*>& outStateLayouts,
            Array<RHIShaderResourceGroupLayout*>& outLayouts)
    {
        Array<uint32_t> groupIndices;
        groupIndices.Reserve(outStateLayouts.GetSize());

        for (auto [group, _]: outStateLayouts)
        {
            groupIndices.Append(group);
        }

        HashMap<uint32_t, RHIShaderResourceGroupLayout*> srgLayoutsMap =
                SPIRVPipelineReflector::BuildLayouts(reflection, m_srgLayoutCache, groupIndices);

        HashMap<uint32_t, RHIShaderResourceGroupLayout*> merged;
        for (auto [group, layout]: srgLayoutsMap)
        {
            groupIndices.Append(group);
            merged.Put(group, layout);
        }

        for (auto [group, layout]: outStateLayouts)
        {
            merged.Put(group, layout);
        }

        outStateLayouts.Clear();
        outStateLayouts = merged;

        outLayouts.Reserve(merged.GetSize());
        // Critical section: we need to ensure the order of the shader resource group layouts is consistent, otherwise
        // pipeline creation will fail.
        std::sort(groupIndices.begin(), groupIndices.end());
        for (uint32_t group: groupIndices)
        {
            outLayouts.Append(merged.Get(group));
        }
    }

}// namespace Wl
