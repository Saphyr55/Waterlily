#pragma once

#include "ACP/ACPExports.hpp"
#include "ACP/AssetImporter.hpp"

namespace Wl
{

    class WL_TOOLS_ACP_API GlTFImporter : public AssetImporter
    {
    public:
        virtual SharedPtr<Asset> ImportAsset(ImportContext& context) override;

    public:
        GlTFImporter(AssetImporterRegistry& importers)
            : m_importers(importers)
        {
        }

    private:
        AssetImporterRegistry& m_importers;
    };

}// namespace Wl
