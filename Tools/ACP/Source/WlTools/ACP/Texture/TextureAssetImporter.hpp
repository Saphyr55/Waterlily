#pragma once

#include "WlTools/ACP/ACPExports.hpp"
#include "WlTools/ACP/AssetImporter.hpp"

namespace Wl
{

    class WL_TOOLS_ACP_API TextureAssetImporter : public AssetImporter
    {
    public:
        virtual SharedPtr<Asset> ImportAsset(ImportContext& context) override;
    };

}// namespace Wl
