#pragma once

#include "Waterlily/Assets/Asset.hpp"
#include "Waterlily/Core/String/StringID.hpp"
#include "Waterlily/Renderer/RendererExports.hpp"

namespace Wl
{

    inline const StringID AssetType_Model = WL_SID("Model");

    struct Model : Asset
    {
        Array<AssetHandle> Meshes;

        Model()
            : Asset(AssetType_Model)
        {
        }

        virtual ~Model() = default;
    };

    WL_RENDERER_API void operator<<(OutputStream& stream, const Model& asset);
    WL_RENDERER_API void operator>>(InputStream& stream, Model& asset);

}// namespace Wl
