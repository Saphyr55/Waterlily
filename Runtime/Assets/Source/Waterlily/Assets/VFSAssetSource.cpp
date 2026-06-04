#include "Waterlily/Assets/VFSAssetSource.hpp"
#include "Waterlily/Core/IO/FileHandle.hpp"
#include "Waterlily/Core/IO/FileSystem.hpp"
#include "Waterlily/Core/Memory/SharedPtr.hpp"

namespace Wl
{

    SharedPtr<Stream> VFSAssetSource::OpenAsset(StringRef location)
    {
        FileResult result = m_filesystem.OpenRead(location);
        if (!result.HasValue())
        {
            return nullptr;
        }

        SharedPtr<FileHandle> handle = result.GetValue();

        return handle;
    }

}// namespace Wl