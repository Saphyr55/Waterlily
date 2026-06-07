#include "Waterlily/Renderer/Shader/ShaderParser.hpp"
#include "Waterlily/Core/Containers/Array.hpp"
#include "Waterlily/Core/Containers/ArrayView.hpp"
#include "Waterlily/Core/Defines.hpp"
#include "Waterlily/Core/Memory/SharedPtr.hpp"
#include "Waterlily/Core/String/StringID.hpp"
#include "Waterlily/Core/String/StringRef.hpp"
#include "Waterlily/Core/Trace/Trace.hpp"
#include "Waterlily/RHI/CompiledShader.hpp"
#include "Waterlily/RHI/ShaderResourceCache.hpp"
#include "Waterlily/RHI/Types.hpp"

#include <algorithm>
#include <spirv_reflect.h>

namespace Wl
{

    static RHIFormat RHIFormatFromSPV(SpvReflectFormat format)
    {
        switch (format)
        {
            case SPV_REFLECT_FORMAT_UNDEFINED:
                return RHIFormat::Undefined;
            case SPV_REFLECT_FORMAT_R16_UINT:
                return RHIFormat::R16_UINT;
            case SPV_REFLECT_FORMAT_R16_SINT:
                return RHIFormat::R16_SINT;
            case SPV_REFLECT_FORMAT_R16_SFLOAT:
                return RHIFormat::R16_FLOAT;
            case SPV_REFLECT_FORMAT_R16G16_UINT:
                return RHIFormat::RG16_UINT;
            case SPV_REFLECT_FORMAT_R16G16_SINT:
                return RHIFormat::RG16_SINT;
            case SPV_REFLECT_FORMAT_R16G16_SFLOAT:
                return RHIFormat::RG16_FLOAT;
            case SPV_REFLECT_FORMAT_R16G16B16_UINT:
                return RHIFormat::RGB16_UINT;
            case SPV_REFLECT_FORMAT_R16G16B16_SINT:
                return RHIFormat::RGB16_SINT;
            case SPV_REFLECT_FORMAT_R16G16B16_SFLOAT:
                return RHIFormat::RGB16_FLOAT;
            case SPV_REFLECT_FORMAT_R16G16B16A16_UINT:
                return RHIFormat::RGBA16_UINT;
            case SPV_REFLECT_FORMAT_R16G16B16A16_SINT:
                return RHIFormat::RGBA16_SINT;
            case SPV_REFLECT_FORMAT_R16G16B16A16_SFLOAT:
                return RHIFormat::RGBA16_FLOAT;
            case SPV_REFLECT_FORMAT_R32_UINT:
                return RHIFormat::R32_UINT;
            case SPV_REFLECT_FORMAT_R32_SINT:
                return RHIFormat::R32_SINT;
            case SPV_REFLECT_FORMAT_R32_SFLOAT:
                return RHIFormat::R32_FLOAT;
            case SPV_REFLECT_FORMAT_R32G32_UINT:
                return RHIFormat::RG32_UINT;
            case SPV_REFLECT_FORMAT_R32G32_SINT:
                return RHIFormat::RG32_SINT;
            case SPV_REFLECT_FORMAT_R32G32_SFLOAT:
                return RHIFormat::RG32_FLOAT;
            case SPV_REFLECT_FORMAT_R32G32B32_UINT:
                return RHIFormat::RGB32_UINT;
            case SPV_REFLECT_FORMAT_R32G32B32_SINT:
                return RHIFormat::RGB32_SINT;
            case SPV_REFLECT_FORMAT_R32G32B32_SFLOAT:
                return RHIFormat::RGB32_FLOAT;
            case SPV_REFLECT_FORMAT_R32G32B32A32_UINT:
                return RHIFormat::RGBA32_UINT;
            case SPV_REFLECT_FORMAT_R32G32B32A32_SINT:
                return RHIFormat::RGBA32_SINT;
            case SPV_REFLECT_FORMAT_R32G32B32A32_SFLOAT:
                return RHIFormat::RGBA32_FLOAT;
            case SPV_REFLECT_FORMAT_R64_UINT:
            case SPV_REFLECT_FORMAT_R64_SINT:
            case SPV_REFLECT_FORMAT_R64_SFLOAT:
            case SPV_REFLECT_FORMAT_R64G64_UINT:
            case SPV_REFLECT_FORMAT_R64G64_SINT:
            case SPV_REFLECT_FORMAT_R64G64_SFLOAT:
            case SPV_REFLECT_FORMAT_R64G64B64_UINT:
            case SPV_REFLECT_FORMAT_R64G64B64_SINT:
            case SPV_REFLECT_FORMAT_R64G64B64_SFLOAT:
            case SPV_REFLECT_FORMAT_R64G64B64A64_UINT:
            case SPV_REFLECT_FORMAT_R64G64B64A64_SINT:
            case SPV_REFLECT_FORMAT_R64G64B64A64_SFLOAT:
                WL_CHECK_MSG(false, "Unsupported format");
                break;
        }
        return RHIFormat::Undefined;
    }

    static uint32_t SrideFromSPV(SpvReflectFormat format)
    {
        switch (format)
        {
            // 16-bit formats
            case SPV_REFLECT_FORMAT_R16_UINT:
            case SPV_REFLECT_FORMAT_R16_SINT:
            case SPV_REFLECT_FORMAT_R16_SFLOAT:
                return 2;

            case SPV_REFLECT_FORMAT_R16G16_UINT:
            case SPV_REFLECT_FORMAT_R16G16_SINT:
            case SPV_REFLECT_FORMAT_R16G16_SFLOAT:
                return 4;

            case SPV_REFLECT_FORMAT_R16G16B16_UINT:
            case SPV_REFLECT_FORMAT_R16G16B16_SINT:
            case SPV_REFLECT_FORMAT_R16G16B16_SFLOAT:
                return 6;

            case SPV_REFLECT_FORMAT_R16G16B16A16_UINT:
            case SPV_REFLECT_FORMAT_R16G16B16A16_SINT:
            case SPV_REFLECT_FORMAT_R16G16B16A16_SFLOAT:
                return 8;

            // 32-bit formats
            case SPV_REFLECT_FORMAT_R32_UINT:
            case SPV_REFLECT_FORMAT_R32_SINT:
            case SPV_REFLECT_FORMAT_R32_SFLOAT:
                return 4;

            case SPV_REFLECT_FORMAT_R32G32_UINT:
            case SPV_REFLECT_FORMAT_R32G32_SINT:
            case SPV_REFLECT_FORMAT_R32G32_SFLOAT:
                return 8;

            case SPV_REFLECT_FORMAT_R32G32B32_UINT:
            case SPV_REFLECT_FORMAT_R32G32B32_SINT:
            case SPV_REFLECT_FORMAT_R32G32B32_SFLOAT:
                return 12;

            case SPV_REFLECT_FORMAT_R32G32B32A32_UINT:
            case SPV_REFLECT_FORMAT_R32G32B32A32_SINT:
            case SPV_REFLECT_FORMAT_R32G32B32A32_SFLOAT:
                return 16;

            // 64-bit formats
            case SPV_REFLECT_FORMAT_R64_UINT:
            case SPV_REFLECT_FORMAT_R64_SINT:
            case SPV_REFLECT_FORMAT_R64_SFLOAT:
                return 8;

            case SPV_REFLECT_FORMAT_R64G64_UINT:
            case SPV_REFLECT_FORMAT_R64G64_SINT:
            case SPV_REFLECT_FORMAT_R64G64_SFLOAT:
                return 16;

            case SPV_REFLECT_FORMAT_R64G64B64_UINT:
            case SPV_REFLECT_FORMAT_R64G64B64_SINT:
            case SPV_REFLECT_FORMAT_R64G64B64_SFLOAT:
                return 24;

            case SPV_REFLECT_FORMAT_R64G64B64A64_UINT:
            case SPV_REFLECT_FORMAT_R64G64B64A64_SINT:
            case SPV_REFLECT_FORMAT_R64G64B64A64_SFLOAT:
                return 32;

            case SPV_REFLECT_FORMAT_UNDEFINED:
            default:
                return 0;
        }
        return 0;
    }

    static RHIShaderResourceType spirv_descriptor_type_to_RHI(SpvReflectDescriptorType spv_type)
    {
        RHIShaderResourceType fallback = RHIShaderResourceType::Uniform;
        switch (spv_type)
        {
            case SPV_REFLECT_DESCRIPTOR_TYPE_UNIFORM_BUFFER:
                return RHIShaderResourceType::Uniform;
            case SPV_REFLECT_DESCRIPTOR_TYPE_SAMPLER:
                return RHIShaderResourceType::Sampler;
            case SPV_REFLECT_DESCRIPTOR_TYPE_SAMPLED_IMAGE:
            case SPV_REFLECT_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER:
                return RHIShaderResourceType::CombinedTextureSampler;
            case SPV_REFLECT_DESCRIPTOR_TYPE_STORAGE_BUFFER:
                return RHIShaderResourceType::StorageBuffer;
            case SPV_REFLECT_DESCRIPTOR_TYPE_STORAGE_IMAGE:
                return RHIShaderResourceType::StorageTexture;
            case SPV_REFLECT_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER:
                WL_CRASH("No mapping found for SPV_REFLECT_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER");
                return fallback;
            case SPV_REFLECT_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC:
                WL_CRASH("No mapping found for SPV_REFLECT_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC");
                return fallback;
            case SPV_REFLECT_DESCRIPTOR_TYPE_ACCELERATION_STRUCTURE_KHR:
                WL_CRASH("No mapping found for SPV_REFLECT_DESCRIPTOR_TYPE_ACCELERATION_STRUCTURE_KHR");
                return fallback;
            case SPV_REFLECT_DESCRIPTOR_TYPE_INPUT_ATTACHMENT:
                WL_CRASH("No mapping found for SPV_REFLECT_DESCRIPTOR_TYPE_INPUT_ATTACHMENT");
                return fallback;
            default:
                WL_CRASH("Unknown SPIRV descriptor type!");
                return fallback;
        };
        WL_CRASH("");
        return fallback;
    }

    HashMap<uint32_t, RHIShaderResourceGroupLayout*> SPIRVPipelineReflector::BuildLayouts(
            const SPIRVPipelineReflection& reflect,
            SharedPtr<RHIShaderResourceGroupLayoutCache> cache,
            ArrayView<uint32_t> externGroups)
    {
        Array<uint32_t> groupIndices;
        for (auto [group, _]: reflect.Groups)
        {
            groupIndices.Append(group);
        }

        HashMap<uint32_t, RHIShaderResourceGroupLayout*> layouts;
        for (uint32_t group: groupIndices)
        {
            // TODO: Find a better way to skip externals group
            if (externGroups.Contains(group))
            {
                continue;
            }

            Array<RHIShaderResourceBinding> rhiBindings;
            for (const SPIRVBinding& binding: reflect.Groups.Get(group))
            {
                rhiBindings.Emplace(binding.Binding, binding.Type, binding.Stage, binding.Count);
            }
            RHIShaderResourceGroupLayoutDescription desc(rhiBindings);
            layouts.Put(group, cache->Obtain(desc));
        }

        return layouts;
    }

    bool SPIRVPipelineReflector::Reflect(SPIRVPipelineReflection& outReflect, ArrayView<SPIRVShader> shaders)
    {
        if (shaders.IsEmpty())
        {
            return false;
        }

        bool result = true;
        for (const SPIRVShader& shader: shaders)
        {
            result = result && Reflect(outReflect, shader);
        }

        return result;
    }

    bool SPIRVPipelineReflector::Reflect(SPIRVPipelineReflection& outReflect, const SPIRVShader& shader)
    {
        SpvReflectResult result;
        auto isSuccess = [&result]() -> bool
        {
            return result == SPV_REFLECT_RESULT_SUCCESS;
        };

        SpvReflectShaderModule spvModule;

        result = spvReflectCreateShaderModule(shader.GetByteCode().GetSize(), shader.GetByteCode().GetData(), &spvModule);
        WL_LOG_ERROR_AND_RETURN_FALSE_WHEN(!isSuccess(), "[SPIRV]", "Impossible to parse the SPIRV shader.");

        SharedPtr<SpvReflectShaderModule> spvModuleGuard(&spvModule, [](SpvReflectShaderModule* m) -> void
        {
            spvReflectDestroyShaderModule(m);
        });

        uint32_t bindingCount = 0;
        result = spvReflectEnumerateDescriptorBindings(&spvModule, &bindingCount, nullptr);
        WL_LOG_ERROR_AND_RETURN_FALSE_WHEN(!isSuccess(), "[SPIRV]", "Impossible to enumerate descriptor bindings.");

        Array<SpvReflectDescriptorBinding*> spvBindings;
        spvBindings.Resize(bindingCount);

        result = spvReflectEnumerateDescriptorBindings(&spvModule, &bindingCount, spvBindings.GetData());
        WL_LOG_ERROR_AND_RETURN_FALSE_WHEN(!isSuccess(), "[SPIRV]", "Impossible to enumerate descriptor bindings.");

        outReflect.EntryPointNames[shader.GetStage()] = String(spvModule.entry_point_name);

        for (size_t i = 0; i < bindingCount; i++)
        {
            SpvReflectDescriptorBinding* spvBinding = spvBindings[i];

            SPIRVBinding binding;
            binding.Name = CreateSID(StringRef(spvBinding->name));
            binding.Binding = spvBinding->binding;
            binding.Count = spvBinding->count;
            binding.Set = spvBinding->set;
            binding.Type = spirv_descriptor_type_to_RHI(spvBinding->descriptor_type);
            binding.Stage = shader.GetStage();

            if (!outReflect.Groups.Contains(binding.Set))
            {
                outReflect.Groups[binding.Set] = Array<SPIRVBinding>();
            }

            Array<SPIRVBinding>& groups = outReflect.Groups[binding.Set];

            auto it = std::find_if(groups.begin(), groups.end(), [&](const SPIRVBinding& existing) -> bool
            {
                return existing.Set == binding.Set && existing.Binding == binding.Binding;
            });

            if (it != groups.end())
            {
                it->Stage |= binding.Stage;
            }
            else
            {
                groups.Append(binding);
            }
        }

        if (shader.GetStage() == RHIShaderStage::Vertex)
        {
            uint32_t inputCount = 0;
            spvReflectEnumerateInputVariables(&spvModule, &inputCount, nullptr);

            Array<SpvReflectInterfaceVariable*> rawInputs;
            rawInputs.Resize(inputCount);

            spvReflectEnumerateInputVariables(&spvModule, &inputCount, rawInputs.GetData());

            for (const SpvReflectInterfaceVariable* var: rawInputs)
            {
                if (var->built_in != -1)
                {
                    continue;
                }

                SPIRVVertexInput vertexInput = {};
                vertexInput.Location = var->location;
                vertexInput.Format = RHIFormatFromSPV(var->format);
                vertexInput.Stride = SrideFromSPV(var->format);
                outReflect.VertexInputs.Append(vertexInput);
            }
        }

        return true;
    }

}// namespace Wl