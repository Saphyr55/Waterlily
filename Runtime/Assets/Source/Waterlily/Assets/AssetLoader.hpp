#pragma once

#include "Waterlily/Assets/AssetMetadata.hpp"
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

}// namespace Wl
