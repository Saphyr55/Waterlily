#include "Waterlily/Assets/AssetLoader.hpp"
#include "Waterlily/Assets/WLCAFile.hpp"
#include "Waterlily/Assets/AssetMetadata.hpp"
#include "Waterlily/Core/IO/File.hpp"
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
