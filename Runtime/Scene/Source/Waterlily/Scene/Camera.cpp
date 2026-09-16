#include "Waterlily/Scene/Camera.hpp"
#include "Waterlily/Core/Math/Vector3.hpp"
#include "Waterlily/Core/Logging/Trace.hpp"


namespace Wl
{

    void Camera::LookAt(const Vector3f& target)
    {
        Forward = Vector3f::Normalize(target - Position);
        Rotation.x = Math::Degrees(Math::Atan2(Forward.x, Forward.z));
        Rotation.y = Math::Degrees(Math::Asin(Forward.y));
        Rotation.y = Math::Clamp(Rotation.y, -89.0f, 89.0f);
        UpdateVectors();
    }

    void Camera::LookAround(float dx, float dy)
    {
        Rotation.x = Math::Mod(Rotation.x + dx * MouseSensitivity, 360.0f);
        Rotation.y = Math::Clamp(Rotation.y + dy * MouseSensitivity, -89.0f, 89.0f);
        UpdateVectors();
    }

    void Camera::UpdateView()
    {
        View = Matrix4f::LookAt(Position, Position + Forward, Up);
    }

    void Camera::UpdateVectors()
    {
        float radYaw = Math::Radians(Rotation.x);
        float radPitch = Math::Radians(Rotation.y);

        Forward.x = Math::Sin(radYaw) * Math::Cos(radPitch);
        Forward.y = Math::Sin(radPitch);
        Forward.z = Math::Cos(radYaw) * Math::Cos(radPitch);
        Forward = Vector3f::Normalize(Forward);

        Right = Vector3f::Normalize(Vector3f::Cross(WorldUp, Forward));
        Up = Vector3f::Normalize(Vector3f::Cross(Forward, Right));
    }

    void Camera::LogDebug()
    {
        WL_LOG_DEBUG("Ludo", "Camera Position: X=%.2f, Y=%.2f, Z=%.2f", Position.x, Position.y, Position.z);
        WL_LOG_DEBUG("Ludo", "Camera Rotation : X=%.2f, Y=%.2f, Z=%.2f", Rotation.x, Rotation.y, Rotation.z);
        WL_LOG_DEBUG("Ludo", "Camera Front : X=%.2f, Y=%.2f, Z=%.2f", Forward.x, Forward.y, Forward.z);
        WL_LOG_DEBUG("Ludo", "Camera Right : X=%.2f, Y=%.2f, Z=%.2f", Right.x, Right.y, Right.z);
        WL_LOG_DEBUG("Ludo", "Camera Up : X=%.2f, Y=%.2f, Z=%.2f", Up.x, Up.y, Up.z);
        WL_LOG_DEBUG("Ludo", "Camera WorldUp : X=%.2f, Y=%.2f, Z=%.2f", WorldUp.x, WorldUp.y, WorldUp.z);
        WL_LOG_DEBUG("Ludo", "Camera Movement Speed : V=%.2f", MovementSpeed);
        WL_LOG_DEBUG("Ludo", "Camera Mouse Sensitivity : V=%.2f", MouseSensitivity);
    }

}// namespace Wl