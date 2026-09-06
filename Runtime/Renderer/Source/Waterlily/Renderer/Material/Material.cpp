#include "Waterlily/Renderer/Material/Material.hpp"

namespace Wl
{

    void operator<<(OutputStream& stream, const MaterialAsset& asset)
    {
        stream << asset.baseColorFactor;
        stream << asset.metallicFactor;
        stream << asset.roughnessFactor;

        stream << asset.baseColor;
        stream << asset.normal;
        stream << asset.emissive;
        stream << asset.occlusion;
        stream << asset.metallicRoughness;
    }

    void operator>>(InputStream& stream, MaterialAsset& asset)
    {
        stream >> asset.baseColorFactor;
        stream >> asset.metallicFactor;
        stream >> asset.roughnessFactor;

        stream >> asset.baseColor;
        stream >> asset.normal;
        stream >> asset.emissive;
        stream >> asset.occlusion;
        stream >> asset.metallicRoughness;
    }

}// namespace Wl
