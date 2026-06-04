#pragma once

#include "Waterlily/Core/IO/Stream.hpp"
#include "Waterlily/Core/Memory/SharedPtr.hpp"
#include "Waterlily/Core/String/StringRef.hpp"

namespace Wl
{

    class AssetSource
    {
    public:
        virtual SharedPtr<Stream> OpenAsset(StringRef location) = 0;
    };

}// namespace Wl