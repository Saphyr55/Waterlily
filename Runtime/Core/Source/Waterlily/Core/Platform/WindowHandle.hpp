#pragma once

#include "Waterlily/Core/String/StringRef.hpp"

namespace Wl
{

    using WindowHandle = uint32;

    struct WindowProperties
    {
        StringRef Title;
        uint32 Width;
        uint32 Height;
        uint32 X;
        uint32 Y;
    };

}// namespace Wl