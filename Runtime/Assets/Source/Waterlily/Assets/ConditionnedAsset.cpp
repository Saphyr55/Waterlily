#include "Waterlily/Assets/ConditionnedAsset.hpp"

#include "Waterlily/Assets/AssetSerializer.hpp"
#include "Waterlily/Core/IO/File.hpp"
#include "Waterlily/Core/IO/FileSystem.hpp"
#include "Waterlily/Core/IO/Stream.hpp"
#include "Waterlily/Core/Memory/SharedPtr.hpp"


namespace Wl
{

    void WLCA::SerializeAsset(SharedPtr<Stream> stream, Asset* asset)
    {
        WLCAHeader header;
        header.AssetType = asset->AssetType;
        *stream << header;
        AssetSerializer::Serialize(*stream, *asset);
    }

    SharedPtr<Stream> ConditionnedAssetLoader::OpenAndValidate(const AssetMetadata& metadata)
    {
        FileResult fileResult = m_vfs.Open(metadata.GetURI().GetText(), FileAccess::Read, FileMode::Open);
        if (!fileResult.HasValue())
        {
            return nullptr;
        }

        SharedPtr<File> file = fileResult.GetValue();
        WLCAHeader header;
        *file >> header;

        if (header.AssetType != metadata.GetAssetType())
        {
            return nullptr;
        }

        return file;
    }

}// namespace Wl