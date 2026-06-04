#pragma once

#include "Waterlily/Core/Memory/SharedPtr.hpp"

namespace Wl
{

    class Asset;
    class MemoryStream;

    class AssetProcessor
    {
    public:
        virtual bool Process(SharedPtr<Asset> asset) = 0;
    };

}// namespace Wl
