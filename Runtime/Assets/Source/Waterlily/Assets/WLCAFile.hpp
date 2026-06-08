#pragma once

#include "Waterlily/Core/String/StringID.hpp"

namespace Wl
{

    constexpr StringRef WLCA_EXTENSION = ".wlca";
    constexpr uint32_t WLCA_FILE_VERSION = 1;
    constexpr uint64_t WLCA_FILE_TYPE = Wl::CStringPack64("WLCA    ");

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

}// namespace Wl
