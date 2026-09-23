#pragma once

#include "Waterlily/Core/Math/Vector3.hpp"

namespace Wl
{

    struct PointLight
    {
        Vector3f Position;
        float _pad0;
        Vector3f Color;
        float Intensity;
    };

    struct DirectionalLight
    {
        alignas(16) Vector3f Direction;
        alignas(16) Vector3f Color;
    };

}// namespace Wl