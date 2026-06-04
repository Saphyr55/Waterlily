#include "Waterlily/Scene/Camera.hpp"
#include "Waterlily/Core/Math/Vector3.hpp"
#include "Waterlily/Core/Platform/Input.hpp"
#include "Waterlily/Core/String/Format.hpp"
#include "Waterlily/Core/Trace/Trace.hpp"

namespace Wl
{

    void Camera::LookAt(const Vector3f& target)
    {
        Front = Vector3f::Normalize(Position - target);
        Rotation.x = Math::Degrees(Math::Atan2(Front.z, Front.x));
        Rotation.y = Math::Degrees(Math::Asin(Front.y));
        Math::Clamp(Rotation.y, -89.0f, 89.0f);
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
        View = Matrix4f::LookAt(Position, Position + Front, Up);
    }

    void Camera::UpdateVectors()
    {
        float radYaw = Math::Radians(Rotation.x);
        float radPitch = Math::Radians(Rotation.y);

        Front.x = Math::Cos(radYaw) * Math::Cos(radPitch);
        Front.y = Math::Sin(radPitch);
        Front.z = Math::Sin(radYaw) * Math::Cos(radPitch);

        Right = Vector3f::Normalize(Vector3f::Cross(Front, WorldUp));
        Up = Vector3f::Normalize(Vector3f::Cross(Front, Right));
    }

    void Camera::LogDebug()
    {
        WL_LOG_DEBUG("[Ludo]", Wl::Format("Camera Position: X=%.2f, Y=%.2f, Z=%.2f", Position.x, Position.y, Position.z));
        WL_LOG_DEBUG("[Ludo]", Wl::Format("Camera Rotation : X=%.2f, Y=%.2f, Z=%.2f", Rotation.x, Rotation.y, Rotation.z));
        WL_LOG_DEBUG("[Ludo]", Wl::Format("Camera Front : X=%.2f, Y=%.2f, Z=%.2f", Front.x, Front.y, Front.z));
        WL_LOG_DEBUG("[Ludo]", Wl::Format("Camera Right : X=%.2f, Y=%.2f, Z=%.2f", Right.x, Right.y, Right.z));
        WL_LOG_DEBUG("[Ludo]", Wl::Format("Camera Up : X=%.2f, Y=%.2f, Z=%.2f", Up.x, Up.y, Up.z));
        WL_LOG_DEBUG("[Ludo]", Wl::Format("Camera World Up : X=%.2f, Y=%.2f, Z=%.2f", WorldUp.x, WorldUp.y, WorldUp.z));
        WL_LOG_DEBUG("[Ludo]", Wl::Format("Camera Movement Speed : V=%.2f", MovementSpeed));
        WL_LOG_DEBUG("[Ludo]", Wl::Format("Camera Mouse Sensitivity : V=%.2f", MouseSensitivity));
    }

}// namespace Wl