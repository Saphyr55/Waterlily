#pragma once

#include "ACP/ACPExports.hpp"
#include "ACP/AssetImporter.hpp"
#include "Waterlily/Assets/Asset.hpp"
#include "Waterlily/Renderer/Model/Model.hpp"

#include <tiny_gltf.h>

namespace Wl
{

    struct GlTFAsset : Model
    {
        tinygltf::Model GlTFModel;

        GlTFAsset()
            : Model()
        {
        }

        virtual ~GlTFAsset() = default;
    };

    WL_TOOLS_ACP_API bool GlTFConstruct(AssetHandle handle,
                                        GlTFAsset& model,
                                        AssetImporterRegistry& importers,
                                        ImportContext& context,
                                        StringRef basedir);

}// namespace Wl