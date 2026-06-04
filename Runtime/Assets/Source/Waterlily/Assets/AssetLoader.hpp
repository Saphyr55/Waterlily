#pragma once

#include "Waterlily/Assets/AssetMetadata.hpp"
#include "Waterlily/Assets/AssetsExports.hpp"
#include "Waterlily/Core/IO/FileSystem.hpp"
#include "Waterlily/Core/IO/Stream.hpp"
#include "Waterlily/Core/Memory/SharedPtr.hpp"

namespace Wl
{

    class IAssetLoader
    {
    public:
        virtual SharedPtr<Stream> OpenAndValidate(const AssetMetadata& metadata) = 0;

        virtual ~IAssetLoader() = default;
    };

    class WL_ASSETS_API LCAAssetLoader : public IAssetLoader
    {
    public:
        virtual SharedPtr<Stream> OpenAndValidate(const AssetMetadata& metadata) override;

        virtual ~LCAAssetLoader() override = default;

        LCAAssetLoader(FileSystem& vfs)
            : m_vfs(vfs)
        {
        }

    private:
        FileSystem& m_vfs;
    };

}// namespace Wl
