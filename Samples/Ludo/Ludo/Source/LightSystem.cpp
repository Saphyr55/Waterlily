#include "LightSystem.hpp"

#include "Waterlily/Core/Platform/PlatformTime.hpp"
#include "Waterlily/Scene/PointLight.hpp"
#include "Waterlily/Scene/SceneComponent.hpp"

namespace Ludo
{

    void RegisterLights(EntityRegistry& registry)
    {
        Entity light1 = registry.Create();
        TransformComponent light1Transform = registry.AddComponent(light1, TransformComponent({7.44f, 22.80f, -2.38f}));
        registry.AddComponent(light1, LightComponent({1.0f, 0.6f, 0.1f}));
        registry.AddComponent(light1, LightAnimationComponent(light1Transform.Position));

        Entity light2 = registry.Create();
        TransformComponent light2Transform = registry.AddComponent(light2, TransformComponent({-10.21f, 33.78f, -25.80f}));
        registry.AddComponent(light2, LightComponent({0.4f, 1.0f, 0.2f}));
        registry.AddComponent(light2, LightAnimationComponent(light2Transform.Position));

        Entity light3 = registry.Create();
        TransformComponent light3Transform = registry.AddComponent(light3, TransformComponent({56.30f, 6.95f, -1.71f}));
        registry.AddComponent(light3, LightComponent({0.0f, 0.5f, 1.0f}));
        registry.AddComponent(light3, LightAnimationComponent(light3Transform.Position));

        Entity light4 = registry.Create();
        TransformComponent light4Transform = registry.AddComponent(light4, TransformComponent({-7.43f, 8.94f, 23.38f}));
        registry.AddComponent(light4, LightComponent({0.4f, 0.8f, 0.8f}));
        registry.AddComponent(light4, LightAnimationComponent(light4Transform.Position));

        Entity directionalLightEntity = registry.Create();
        registry.AddComponent(directionalLightEntity, DirectionalLight {
                .Direction = Vector3f(0.1f, -1.0f, 0.1f),
                .Color = Vector3f(1.0f, 0.90f, 0.75f),  
        });
    }

    void UpdateLights(EntityRegistry& registry, double deltaTime)
    {
        auto lightView = registry.View<TransformComponent, LightComponent, LightAnimationComponent>();

        float epsiledTime = static_cast<float>(PlatformGetHighResolutionTime());

        for (auto [entity, transform, light, anim]: lightView)
        {
            float offset = Math::Sin(epsiledTime * anim.Velocity) * anim.Amplitude;
            transform.Position = anim.BasePosition + anim.Axis * offset;
        }
    }

}// namespace Wl