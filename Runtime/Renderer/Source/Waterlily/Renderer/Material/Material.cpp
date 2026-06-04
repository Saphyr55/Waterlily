#include "Waterlily/Renderer/Material/Material.hpp"

namespace Wl
{

    void operator<<(OutputStream& stream, const MaterialAsset& asset)
    {
        stream << asset.DiffuseFactor;
        stream << asset.Diffuse;
        stream << asset.Normal;
    }

    void operator>>(InputStream& stream, MaterialAsset& asset)
    {
        stream >> asset.DiffuseFactor;
        stream >> asset.Diffuse;
        stream >> asset.Normal;
    }

}// namespace Wl
