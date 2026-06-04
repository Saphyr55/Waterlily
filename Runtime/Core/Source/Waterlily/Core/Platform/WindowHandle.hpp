#pragma once

#include "Waterlily/Core/Defines.hpp"
#include "Waterlily/Core/String/StringRef.hpp"

namespace Wl
{

    using WindowHandle = uint32_t;

    struct WindowProperties
    {
        StringRef Title;
        float Width;
        float Height;
        float X;
        float Y;
    };

}// namespace Wl