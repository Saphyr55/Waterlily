#include "Waterlily/Assets/AssetLoader.hpp"
#include "Waterlily/Assets/LCAFile.hpp"
#include "Waterlily/Assets/assetMetadata.hpp"
#include "Waterlily/Core/IO/FileHandle.hpp"
#include "Waterlily/Core/IO/Stream.hpp"

namespace Wl
{

    SharedPtr<Stream> LCAAssetLoader::OpenAndValidate(const AssetMetadata& metadata)
    {
        FileResult fileResult = m_vfs.Open(metadata.GetURI().GetText(), FileAccess::Read, FileMode::Open);
        if (!fileResult.HasValue())
        {
            return nullptr;
        }

        SharedPtr<FileHandle> file = fileResult.GetValue();
        LCAHeader header;
        *file >> header;

        if (header.AssetType != metadata.GetAssetType())
        {
            return nullptr;
        }

        return file;
    }

}// namespace Wl
