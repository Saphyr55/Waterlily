#pragma once

#include "Waterlily/Assets/AssetMetadata.hpp"
#include "Waterlily/Core/IO/Stream.hpp"
#include "Waterlily/Core/Memory/SharedPtr.hpp"

namespace Wl
{

    class AssetLoader
    {
    public:
        virtual SharedPtr<Stream> OpenAndValidate(const AssetMetadata& metadata) = 0;

        virtual ~AssetLoader() = default;
    };

}// namespace Wl
