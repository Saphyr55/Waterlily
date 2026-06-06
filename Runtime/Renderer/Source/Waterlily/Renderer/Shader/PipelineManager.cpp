#include "Waterlily/Renderer/Shader/PipelineManager.hpp"
#include "Waterlily/Core/Containers/Array.hpp"
#include "Waterlily/Core/Defines.hpp"
#include "Waterlily/Core/IO/FileHandle.hpp"
#include "Waterlily/Core/String/Format.hpp"
#include "Waterlily/Core/String/StringID.hpp"
#include "Waterlily/RHI/GraphicsPipeline.hpp"
#include "Waterlily/RHI/ShaderResource.hpp"
#include "Waterlily/Renderer/Shader/ShaderParser.hpp"

namespace Wl
{

    RHIGraphicsPipeline* PipelineManager::Create(const StringID& name, GraphicsPipelineProperties& props)
    {
        WL_CHECK_MSG(!m_cache.Contains(name), Wl::Format("The pipeline named '%s' already exist.", name.GetText().data()));

        RHIGraphicsPipeline* pipeline = CreateInternal(props);
        m_cache[name] = pipeline;
        return pipeline;
    }

    RHIGraphicsPipeline* PipelineManager::GetOrCreate(const StringID& name, GraphicsPipelineProperties& props)
    {
        if (RHIGraphicsPipeline* pipeline = GetPipeline(name))
        {
            return pipeline;
        }
        return Create(name, props);
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

    RHIGraphicsPipeline* PipelineManager::Recreate(const StringID& name, GraphicsPipelineProperties& props)
    {
        Destroy(name);
        return Create(name, props);
    }

    void PipelineManager::Destroy(const StringID& name)
    {
        RHIGraphicsPipeline* pipeline = GetPipeline(name);
        m_cache.Remove(name);
        DestroyInternal(pipeline);
    }

    void PipelineManager::DestroyInternal(RHIGraphicsPipeline* pipeline)
    {
        m_device->DestroyGraphicsPipeline(pipeline);
    }

    RHIGraphicsPipeline* PipelineManager::CreateInternal(GraphicsPipelineProperties& props)
    {
        WL_CHECK_MSG(props.RenderPass, "Render pass is required to create a graphics pipeline.");
        
        bool result = false;

        // Load shaders binary codes.
        FileResult vertexFileOpenError = m_fileSystem.OpenRead(props.VertexShaderPath.GetText());
        WL_RETURN_OBJECT_WHEN(!vertexFileOpenError.HasValue(), nullptr);

        SharedPtr<FileHandle> vertexFileHandle = vertexFileOpenError.GetValue();
        SPIRVShader vertexShader(RHIShaderStage::Vertex, vertexFileHandle->ReadAllBytes());
        vertexFileHandle->Close();

        FileResult fragmentFileOpenError = m_fileSystem.OpenRead(props.FragmentShaderPath.GetText());
        WL_RETURN_OBJECT_WHEN(!fragmentFileOpenError.HasValue(), nullptr);

        SharedPtr<FileHandle> FragmentFileHandle = fragmentFileOpenError.GetValue();
        SPIRVShader fragmentShader(RHIShaderStage::Fragment, FragmentFileHandle->ReadAllBytes());
        FragmentFileHandle->Close();

        // Reflect shaders to Build shader resource group layouts.
        SPIRVPipelineReflection reflection;
        FixedArray<SPIRVShader, 2> shaders = {vertexShader, fragmentShader};
        result = SPIRVPipelineReflector::Reflect(reflection, shaders);
        WL_RETURN_OBJECT_WHEN(!result, nullptr);

        // Build Shader resource group layouts.
        Array<uint32_t> groupIndices;
        groupIndices.Reserve(props.SRGLayouts.GetSize());

        for (const auto& [group, _]: props.SRGLayouts)
        {
            groupIndices.Append(group);
        }

        HashMap<uint32_t, RHIShaderResourceGroupLayout*> srgLayoutsMap =
                SPIRVPipelineReflector::BuildLayouts(reflection, m_srgLayoutCache, groupIndices);

        HashMap<uint32_t, RHIShaderResourceGroupLayout*> merged;
        for (const auto& [group, layout]: srgLayoutsMap)
        {
            groupIndices.Append(group);
            merged.Put(group, layout);
        }

        for (const auto& [group, layout]: props.SRGLayouts)
        {
            merged.Put(group, layout);
        }

        props.SRGLayouts.Clear();
        props.SRGLayouts = merged;

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
        builder.WithRenderPass(props.RenderPass)
                .WithVertexShader(vertexShader, "main")
                .WithFragmentShader(fragmentShader, "main")
                .WithVertexBindings(bindings, attributes)
                .WithViewport(props.Viewport, props.Scissor)
                .WithSRGLayout(srgLayouts)
                .WithCullMode(props.CullMode);

        const RHIGraphicsPipelineDescription& description = builder.Build();
        return m_device->CreateGraphicsPipeline(description);
    }

}// namespace Wl
