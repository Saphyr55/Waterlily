#pragma once

#include "Waterlily/Core/Math/Vector3.hpp"
#include "Waterlily/Entity/Entity.hpp"
#include "Waterlily/Scene/SceneComponent.hpp"

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

    inline void RegisterLights(EntityRegistry& registry)
    {
        Entity light1 = registry.Create();
        TransformComponent light1Transform = registry.AddComponent(light1, TransformComponent({-5.76f, 1.22f, -0.82f}));
        registry.AddComponent(light1, LightComponent({1.0f, 0.6f, 0.1f}));
        registry.AddComponent(light1, LightAnimationComponent(light1Transform.Position));

        Entity light2 = registry.Create();
        TransformComponent light2Transform = registry.AddComponent(light2, TransformComponent({-0.46f, 3.63f, 1.88f}));
        registry.AddComponent(light2, LightComponent({0.4f, 1.0f, 0.2f}));
        registry.AddComponent(light2, LightAnimationComponent(light2Transform.Position));

        Entity light3 = registry.Create();
        TransformComponent light3Transform = registry.AddComponent(light3, TransformComponent({-2.07f, 1.94f, 0.04f}));
        registry.AddComponent(light3, LightComponent({0.0f, 0.5f, 1.0f}));
        registry.AddComponent(light3, LightAnimationComponent(light3Transform.Position));

        Entity light4 = registry.Create();
        TransformComponent light4Transform = registry.AddComponent(light4, TransformComponent({-6.5f, 0.75f, 0.5f}));
        registry.AddComponent(light4, LightComponent({0.4f, 0.8f, 0.8f}));
        registry.AddComponent(light4, LightAnimationComponent(light4Transform.Position));
    }

}// namespace Wl