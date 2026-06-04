#pragma once

#include "Waterlily/Core/Math/Matrix4.hpp"
#include "Waterlily/Core/Math/Vector3.hpp"
#include "Waterlily/Scene/SceneExports.hpp"

namespace Wl
{

    struct WL_SCENE_API Camera
    {
    public:
        Vector3f Rotation;
        Vector3f Position;
        Vector3f Front;
        Vector3f Right = Vector3f(-1.0f, 0.0f, 0.0f);
        Vector3f Up;
        Vector3f WorldUp = Vector3f(0.0f, 1.0f, 0.0f);

        Matrix4f View = Matrix4f(1.0f);

        float MovementSpeed = 1.0f;
        float MouseSensitivity = 0.15f;

    public:
        void LookAt(const Vector3f& target);

        void LookAround(float dx, float dy);

        void UpdateView();
        void UpdateVectors();

        void LogDebug();

    public:
        Camera(const Vector3f& position, const Vector3f& world_up = Vector3f(0.0f, 1.0f, 0.0f))
            : Position(position)
            , WorldUp(world_up)
            , Rotation(0.0f, 0.0f, 0.0f)
        {
            UpdateVectors();
        }
    };

}// namespace Wl