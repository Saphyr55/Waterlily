#include "ACP/AssetImporter.hpp"

namespace Wl
{

    void AssetImporterRegistry::RegisterImporter(StringID type, const SharedPtr<AssetImporter>& importer)
    {
        m_importers[type] = importer;
    }

    SharedPtr<AssetImporter> AssetImporterRegistry::GetImporter(StringID type)
    {
        return m_importers[type];
    }

}// namespace Wl
