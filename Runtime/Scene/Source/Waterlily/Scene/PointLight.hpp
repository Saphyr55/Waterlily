#pragma once

#include "Waterlily/Core/Math/Vector3.hpp"

namespace Wl
{

    struct PointLight
    {
        alignas(16) Vector3f Position;
        alignas(16) Vector3f Color;
    };

    struct DirectionalLight
    {
        alignas(16) Vector3f Direction;
        alignas(16) Vector3f Color;
    };

}// namespace Wl