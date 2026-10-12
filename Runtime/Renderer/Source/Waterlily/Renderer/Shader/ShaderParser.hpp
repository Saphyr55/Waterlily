#pragma once

#include "Waterlily/Core/Containers/Array.hpp"
#include "Waterlily/Core/Containers/ArrayView.hpp"
#include "Waterlily/Core/Containers/HashMap.hpp"
#include "Waterlily/Core/String/StringID.hpp"
#include "Waterlily/RHI/CompiledShader.hpp"
#include "Waterlily/RHI/ShaderResource.hpp"
#include "Waterlily/RHI/ShaderResourceCache.hpp"
#include "Waterlily/RHI/Types.hpp"
#include "Waterlily/Renderer/RendererExports.hpp"

namespace Wl
{

    struct SPIRVBinding
    {
        StringID Name;
        uint32 Binding;
        uint32 Set;
        uint32 Count = 1;
        RHIShaderResourceType Type;
        RHIShaderStage Stage = RHIShaderStage::AllGraphics;
    };

    struct SPIRVVertexInput
    {
        RHIFormat Format;
        uint32 Location;
        uint32 Stride;
    };

    struct SPIRVPipelineReflection
    {
        HashMap<RHIShaderStage, String> EntryPointNames;
        HashMap<uint32, Array<SPIRVBinding>> Groups;
        Array<SPIRVVertexInput> VertexInputs;
    };

    class WL_RENDERER_API SPIRVPipelineReflector
    {
    public:
        static bool Reflect(SPIRVPipelineReflection& outReflect, ArrayView<SPIRVShader> shaders);
        static bool Reflect(SPIRVPipelineReflection& outReflect, const SPIRVShader& shader);

        static HashMap<uint32, RHIShaderResourceGroupLayout*> BuildLayouts(const SPIRVPipelineReflection& reflect,
                                                                             RHIShaderResourceGroupLayoutCache& cache,
                                                                             ArrayView<uint32> externGroups);
    };

}// namespace Wl
