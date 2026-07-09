#pragma once

#include "Waterlily/Core/Math/Vector3.hpp"
#include "Waterlily/Entity/EntityRegistry.hpp"

namespace Wl
{

    struct LightComponent
    {
        Vector3f Color;
        float Intensity = 1.0f;
    };

    struct LightAnimationComponent
    {
        Vector3f BasePosition;
        Vector3f Axis = Vector3f(1.0f, 0.0f, 0.0f);
        float Amplitude = 0.5f;
        float Velocity = 3.0f;
    };

    void RegisterLights(EntityRegistry& registry);
    
    void UpdateLights(EntityRegistry& registry, double deltaTime);

}// namespace Wl