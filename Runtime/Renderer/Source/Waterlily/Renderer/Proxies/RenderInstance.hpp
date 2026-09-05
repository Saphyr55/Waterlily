#pragma once

#include "Waterlily/Core/Math/Matrix4.hpp"
#include "Waterlily/Renderer/Material/MaterialRegistry.hpp"
#include "Waterlily/Renderer/RendererExports.hpp"

namespace Wl
{

    struct RenderSubMesh;
    struct RenderSubMeshData;

    struct WL_RENDERER_API RenderInstanceLayout
    {
        size_t ModelOffset;
        size_t MaterialOffset;
        size_t Stride;

        void UpdateData(uint8_t* dst, const RenderSubMesh& src);
    };

    struct WL_RENDERER_API RenderInstance
    {
        Matrix4f Model = Matrix4f::Identity();
        MaterialHandle Material = MaterialRegistry::InvalidHandle;

        static RenderInstanceLayout CreateLayout(size_t minAlignment);
    };

}// namespace Wl