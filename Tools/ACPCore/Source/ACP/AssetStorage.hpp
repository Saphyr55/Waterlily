#pragma once

#include "Waterlily/Assets/Asset.hpp"
#include "Waterlily/Core/Memory/SharedPtr.hpp"

namespace Wl
{

    using AssetStorage = HashMap<AssetHandle, SharedPtr<Asset>>;

}