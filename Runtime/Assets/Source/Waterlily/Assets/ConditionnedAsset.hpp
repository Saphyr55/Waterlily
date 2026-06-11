#pragma once

#include "Waterlily/Assets/AssetLoader.hpp"
#include "Waterlily/Assets/AssetsExports.hpp"
#include "Waterlily/Core/Memory/SharedPtr.hpp"
#include "Waterlily/Core/String/StringID.hpp"
#include "Waterlily/Core/String/StringRef.hpp"

namespace Wl
{

    constexpr StringRef WLCA_EXTENSION = ".wlca";
    constexpr uint32_t WLCA_FILE_VERSION = 1;
    constexpr uint64_t WLCA_FILE_TYPE = Wl::CStringPack64("WLCA    ");

    class FileSystem;

    struct WLCAHeader
    {
        uint64_t Filetype = WLCA_FILE_TYPE;
        uint32_t Version = WLCA_FILE_VERSION;
        StringID AssetType;
    };

    inline void operator<<(OutputStream& stream, const WLCAHeader& header)
    {
        stream << header.Filetype;
        stream << header.Version;
        stream << header.AssetType;
    }

    inline void operator>>(InputStream& stream, WLCAHeader& header)
    {
        stream >> header.Filetype;
        stream >> header.Version;
        stream >> header.AssetType;
    }

    class WL_ASSETS_API WLCA
    {
    public:
        static void SerializeAsset(SharedPtr<Stream> stream, Asset* asset);
    };

    class WL_ASSETS_API ConditionnedAssetLoader : public IAssetLoader
    {
    public:
        virtual SharedPtr<Stream> OpenAndValidate(const AssetMetadata& metadata) override;

        virtual ~ConditionnedAssetLoader() override = default;

    public:
        ConditionnedAssetLoader(FileSystem& vfs)
            : m_vfs(vfs)
        {
        }

    private:
        FileSystem& m_vfs;
    };

}// namespace Wl
