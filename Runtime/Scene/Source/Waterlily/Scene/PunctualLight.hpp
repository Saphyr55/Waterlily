#pragma once

#include "Waterlily/Core/Math/Vector3.hpp"

namespace Wl
{

    struct PunctualLight
    {
        alignas(16) Vector3f Position;
        alignas(16) Vector3f Color;
    };

}// namespace Wl