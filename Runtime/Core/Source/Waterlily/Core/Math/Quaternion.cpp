#include "Waterlily/Core/Math/Quaternion.hpp"
#include "Waterlily/Core/Defines.hpp"
#include "Waterlily/Core/Math/Math.hpp"

namespace Wl
{

    constexpr Quaternion Quaternion::FromAxisAngle(const Vector3f& axis, float angle)
    {
        const float halfAngle = 0.5f * angle;
        float halfAngleSin = Math::Sin(halfAngle);
        float halfAngleCos = Math::Cos(halfAngle);

        return {halfAngleSin * axis.x, halfAngleSin * axis.y, halfAngleSin * axis.z, halfAngleCos};
    }

    constexpr Matrix4f Quaternion::RotationMatrix(const Quaternion& q)
    {
        Matrix4f result = Matrix4f::Identity();
        float qxx(q.x * q.x);
        float qyy(q.y * q.y);
        float qzz(q.z * q.z);
        float qxz(q.x * q.z);
        float qxy(q.x * q.y);
        float qyz(q.y * q.z);
        float qwx(q.w * q.x);
        float qwy(q.w * q.y);
        float qwz(q.w * q.z);

        result[0][0] = float(1) - float(2) * (qyy + qzz);
        result[0][1] = float(2) * (qxy + qwz);
        result[0][2] = float(2) * (qxz - qwy);

        result[1][0] = float(2) * (qxy - qwz);
        result[1][1] = float(1) - float(2) * (qxx + qzz);
        result[1][2] = float(2) * (qyz + qwx);

        result[2][0] = float(2) * (qxz + qwy);
        result[2][1] = float(2) * (qyz - qwx);
        result[2][2] = float(1) - float(2) * (qxx + qyy);

        return result;
    }

    constexpr Quaternion::Quaternion(float x, float y, float z, float w)
        : x(x)
        , y(y)
        , z(z)
        , w(w)
    {
    }

    constexpr float Quaternion::Normal(const Quaternion& q)
    {
        return Math::Sqrt(q.x * q.x + q.y * q.y + q.z * q.z + q.w * q.w);
    }

    constexpr Quaternion Quaternion::Normalize(const Quaternion& quaternion)
    {
        float quaternionNorm = Normal(quaternion);
        return {quaternion.x / quaternionNorm,
                quaternion.y / quaternionNorm,
                quaternion.z / quaternionNorm,
                quaternion.w / quaternionNorm};
    }

    constexpr Quaternion Quaternion::Conjugate(const Quaternion& quaternion)
    {
        return {-quaternion.x, -quaternion.y, -quaternion.z, quaternion.w};
    }

    constexpr Quaternion Quaternion::operator*(const Quaternion& q) const
    {
        const float newX = x * q.w + w * q.x + y * q.z - z * q.y;
        const float newY = y * q.w + w * q.y + z * q.x - x * q.z;
        const float newZ = z * q.w + w * q.z + x * q.y - y * q.x;
        const float newW = w * q.w - x * q.x - y * q.y - z * q.z;

        return {newX, newY, newZ, newW};
    }

    constexpr Quaternion Quaternion::operator*(const Vector3f& vec) const
    {
        const float newX = w * vec.x + y * vec.z - z * vec.y;
        const float newY = w * vec.y + z * vec.x - x * vec.z;
        const float newZ = w * vec.z + x * vec.y - y * vec.x;
        const float newW = -x * vec.x - y * vec.y - z * vec.z;

        return {newX, newY, newZ, newW};
    }

}// namespace Wl