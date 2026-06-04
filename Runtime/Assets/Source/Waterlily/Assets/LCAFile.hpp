#pragma once

#include "Waterlily/Core/Defines.hpp"
#include "Waterlily/Core/String/StringID.hpp"

namespace Wl
{

    constexpr StringRef LCA_EXTENSION = ".lca";
    constexpr uint32_t LCA_FILE_VERSION = 1;
    constexpr uint64_t LCA_FILE_TYPE = Wl::CStringPack64("LCA     ");

    struct LCAHeader
    {
        uint64_t Filetype = LCA_FILE_TYPE;
        uint32_t Version = LCA_FILE_VERSION;
        StringID AssetType;
    };

    inline void operator<<(OutputStream& stream, const LCAHeader& header)
    {
        stream << header.Filetype;
        stream << header.Version;
        stream << header.AssetType;
    }

    inline void operator>>(InputStream& stream, LCAHeader& header)
    {
        stream >> header.Filetype;
        stream >> header.Version;
        stream >> header.AssetType;
    }

}// namespace Wl
