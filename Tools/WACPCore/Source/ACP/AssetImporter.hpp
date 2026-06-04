#pragma once

#include "ACP/ACPExports.hpp"
#include "AssetStorage.hpp"
#include "Waterlily/Assets/Asset.hpp"
#include "Waterlily/Assets/AssetRegistry.hpp"
#include "Waterlily/Assets/AssetSource.hpp"
#include "Waterlily/Core/Containers/HashMap.hpp"
#include "Waterlily/Core/Memory/SharedPtr.hpp"
#include "Waterlily/Core/String/StringID.hpp"
#include "Waterlily/Core/String/StringRef.hpp"

namespace Wl
{

    class Asset;

    struct ImportContext
    {
        SharedPtr<AssetSource> Source;
        SharedPtr<AssetRegistry> Registry;
        AssetStorage& Storage;
        StringRef AssetType;
        StringRef URI;
        StringRef OutputURI;

        ImportContext(const SharedPtr<AssetSource>& source,
                      const SharedPtr<AssetRegistry>& registry,
                      AssetStorage& storage,
                      StringRef assetType,
                      StringRef uri,
                      StringRef outputURI)
            : Source(source)
            , Registry(registry)
            , Storage(storage)
            , AssetType(assetType)
            , URI(uri)
            , OutputURI(outputURI)
        {
        }
    };

    class AssetImporter
    {
    public:
        virtual SharedPtr<Asset> ImportAsset(ImportContext& context) = 0;
    };

    class WL_TOOLS_ACP_API AssetImporterRegistry
    {
    public:
        void RegisterImporter(StringID format, const SharedPtr<AssetImporter>& importer);

        SharedPtr<AssetImporter> GetImporter(StringID format);

    private:
        HashMap<StringID, SharedPtr<AssetImporter>> m_importers;
    };

}// namespace Wl
