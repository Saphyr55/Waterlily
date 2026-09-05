#pragma once

#include "Waterlily/Core/String/StringRef.hpp"

namespace Wl
{

    using WindowHandle = uint32_t;

    struct WindowProperties
    {
        StringRef Title;
        uint32_t Width;
        uint32_t Height;
        uint32_t X;
        uint32_t Y;
    };

}// namespace Wl