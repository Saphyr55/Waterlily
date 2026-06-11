#include "Waterlily/Renderer/Shader/PipelineManager.hpp"
#include "Waterlily/Core/Containers/Array.hpp"
#include "Waterlily/Core/Defines.hpp"
#include "Waterlily/Core/IO/File.hpp"
#include "Waterlily/Core/Logging/Trace.hpp"
#include "Waterlily/Core/String/Format.hpp"
#include "Waterlily/Core/String/StringID.hpp"
#include "Waterlily/RHI/GraphicsPipeline.hpp"
#include "Waterlily/RHI/ShaderResource.hpp"
#include "Waterlily/Renderer/Shader/ShaderParser.hpp"

namespace Wl
{

    RHIGraphicsPipeline* PipelineManager::Create(const StringID& name, GraphicsPipelineState& props)
    {
        WL_CHECK_MSG(!m_cache.Contains(name), "The pipeline named '%s' already exist.", name.GetText().data());

        RHIGraphicsPipeline* pipeline = CreateInternal(props);
        m_cache[name] = pipeline;
        return pipeline;
    }

    RHIGraphicsPipeline* PipelineManager::GetOrCreate(const StringID& name, GraphicsPipelineState& state)
    {
        if (RHIGraphicsPipeline* pipeline = GetPipeline(name))
        {
            return pipeline;
        }
        return Create(name, state);
    }

    RHIGraphicsPipeline* PipelineManager::GetPipeline(const StringID& name)
    {
        HashMap<StringID, RHIGraphicsPipeline*>::iterator it = m_cache.find(name);
        if (it == m_cache.end())
        {
            return nullptr;
        }
        return (*it).Value;
    }

    RHIGraphicsPipeline* PipelineManager::Recreate(const StringID& name, GraphicsPipelineState& props)
    {
        Destroy(name);
        return Create(name, props);
    }

    void PipelineManager::Destroy(const StringID& name)
    {
        RHIGraphicsPipeline* pipeline = GetPipeline(name);
        WL_CHECK_MSG(pipeline, "Pipeline \"%s\" not found, maybe was already removed?", name.GetText().GetData());
        m_cache.Remove(name);
        DestroyInternal(pipeline);
    }

    void PipelineManager::DestroyInternal(RHIGraphicsPipeline* pipeline)
    {
        WL_CHECK(pipeline);
        m_device->DestroyGraphicsPipeline(pipeline);
    }

    RHIGraphicsPipeline* PipelineManager::CreateInternal(GraphicsPipelineState& state)
    {
        WL_CHECK_MSG(state.RenderPass, "The GraphicsPipelineProperties.RenderPass is not nullable to create a graphics pipeline.");

        bool result = false;

        const SPIRVShader& vertexShader = state.VertexShader->GetSPIRVShader();
        const SPIRVShader& fragmentShader = state.FragmentShader->GetSPIRVShader();

        SPIRVPipelineReflection reflection;
        result = SPIRVPipelineReflector::Reflect(reflection, {vertexShader, fragmentShader});
        WL_RETURN_OBJECT_WHEN(!result, nullptr);

        Array<uint32_t> groupIndices;
        groupIndices.Reserve(state.SRGLayouts.GetSize());

        for (auto [group, _]: state.SRGLayouts)
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

        for (auto [group, layout]: state.SRGLayouts)
        {
            merged.Put(group, layout);
        }

        state.SRGLayouts.Clear();
        state.SRGLayouts = merged;

        Array<RHIShaderResourceGroupLayout*> srgLayouts;
        srgLayouts.Reserve(merged.GetSize());
        // Critical section: we need to ensure the order of the shader resource group layouts is consistent, otherwise
        // pipeline creation will fail.
        std::sort(groupIndices.begin(), groupIndices.end());
        for (uint32_t group: groupIndices)
        {
            srgLayouts.Append(merged.Get(group));
        }

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

}// namespace Wl
