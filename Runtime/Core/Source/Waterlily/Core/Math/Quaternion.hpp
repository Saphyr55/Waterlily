#pragma once

#include "Waterlily/Core/CoreExports.hpp"
#include "Waterlily/Core/Defines.hpp"
#include "Waterlily/Core/Math/Matrix4.hpp"
#include "Waterlily/Core/Math/Vector3.hpp"

namespace Wl
{

    struct WL_CORE_API Quaternion
    {
        float x = 0.0f;
        float y = 0.0f;
        float z = 0.0f;
        float w = 1.0f;

        static constexpr float Normal(const Quaternion& quaternion);

        static constexpr Quaternion Normalize(const Quaternion& quaternion);

        static constexpr Quaternion Conjugate(const Quaternion& quaternion);

        static constexpr Matrix4f RotationMatrix(const Quaternion& q);

        static constexpr Quaternion FromAxisAngle(const Vector3f& axis, float angle);

        constexpr Quaternion operator*(const Quaternion& quaternion) const;
        constexpr Quaternion operator*(const Vector3f& vec) const;

        constexpr Quaternion() = default;
        constexpr Quaternion(float x, float y, float z, float w);
        ~Quaternion() = default;
    };

}// namespace Wl