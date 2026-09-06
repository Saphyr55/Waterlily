#pragma once

#include "ACP/ACPExports.hpp"
#include "ACP/AssetImporter.hpp"

namespace Wl
{

    class WL_TOOLS_ACP_API TextureAssetImporter : public AssetImporter
    {
    public:
        virtual SharedPtr<Asset> ImportAsset(ImportContext& context) override;
    };

}// namespace Wl
