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
        uint32_t Binding;
        uint32_t Set;
        uint32_t Count = 1;
        RHIShaderResourceType Type;
        RHIShaderStage Stage = RHIShaderStage::AllGraphics;
    };

    struct SPIRVVertexInput
    {
        RHIFormat Format;
        uint32_t Location;
        uint32_t Stride;
    };

    struct SPIRVPipelineReflection
    {
        HashMap<uint32_t, Array<SPIRVBinding>> Groups;
        Array<SPIRVVertexInput> VertexInputs;
    };

    class WL_RENDERER_API SPIRVPipelineReflector
    {
    public:
        static bool Reflect(SPIRVPipelineReflection& outReflect, ArrayView<SPIRVShader> shaders);
        static bool Reflect(SPIRVPipelineReflection& outReflect, const SPIRVShader& shader);

        static HashMap<uint32_t, RHIShaderResourceGroupLayout*> BuildLayouts(
                const SPIRVPipelineReflection& reflect,
                SharedPtr<RHIShaderResourceGroupLayoutCache> cache,
                ArrayView<uint32_t> externGroups);
    };

}// namespace Wl
