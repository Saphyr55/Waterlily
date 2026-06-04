#pragma once

#include "Waterlily/Assets/AssetSource.hpp"
#include "Waterlily/Assets/assetsExports.hpp"
#include "Waterlily/Core/IO/FileSystem.hpp"

using namespace Wl;

namespace Wl
{

    class WL_ASSETS_API VFSAssetSource : public AssetSource
    {
    public:
        virtual SharedPtr<Stream> OpenAsset(StringRef location) override;

    public:
        VFSAssetSource(FileSystem& filesystem)
            : m_filesystem(filesystem)
        {
        }

    private:
        FileSystem& m_filesystem;
    };

}// namespace Wl
