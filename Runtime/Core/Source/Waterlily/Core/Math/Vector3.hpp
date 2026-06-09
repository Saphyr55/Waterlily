#pragma once

#include "Waterlily/Core/Math/Math.hpp"
#include "Waterlily/Core/Math/Vector2.hpp"

namespace Wl
{

    template<Real R = float>
    struct Vector3
    {
        R x = 0;
        R y = 0;
        R z = 0;

        constexpr Vector3() = default;
        constexpr Vector3(R x, R y, R z);
        constexpr Vector3(Vector2<R> vec, R z);
        constexpr explicit Vector3(R r);

        R Dot(const Vector3& vec) const;

        Vector3 operator*(const R&) const;
        Vector3 operator+(const R&) const;
        Vector3 operator-(const R&) const;
        Vector3 operator*(const Vector3& vec) const;
        Vector3 operator+(const Vector3& vec) const;
        Vector3 operator-(const Vector3& vec) const;
        Vector3 operator-() const;

        Vector3& operator+=(const Vector3& vec);
        Vector3& operator-=(const Vector3& vec);
        Vector3& operator*=(const Vector3& vec);
        Vector3& operator+=(const R& t);
        Vector3& operator-=(const R& t);
        Vector3& operator*=(const R& t);
        Vector3& operator/=(const R& t);

        constexpr bool operator==(const Vector3& other) const
        {
            return x == other.x && y == other.y && z == other.z;
        }

        constexpr bool operator!=(const Vector3& other) const
        {
            return !(*this == other);
        }

        static constexpr Vector3 Forward();
        static constexpr float LengthSquared(const Vector3<R>& vec);
        static constexpr float Length(const Vector3<R>& vec);
        static constexpr Vector3 Normalize(const Vector3<R>& vec);
        static constexpr Vector3 Cross(const Vector3<R>& v1, const Vector3<R>& v2);
        static constexpr auto Dot(const Vector3& v1, const Vector3& v2);
    };

    using Vector3f = Vector3<float>;
    using Vector3f32 = Vector3<float>;
    using Vector3f64 = Vector3<double>;
    using Vector3i = Vector3<int32_t>;
    using Vector3u = Vector3<uint32_t>;

    template<Real R>
    constexpr Vector3<R>::Vector3(Vector2<R> vec, R z)
        : Vector3(vec.x, vec.y, z)
    {
    }

    template<Real R>
    constexpr Vector3<R>::Vector3(R x, R y, R z)
        : x(x)
        , y(y)
        , z(z)
    {
    }

    template<Real R>
    constexpr Vector3<R>::Vector3(R r)
        : Vector3(r, r, r)
    {
    }

    template<Real R>
    constexpr Vector3<R> Vector3<R>::Forward()
    {
        return Vector3<R>(R(0.0), R(0.0), R(1.0));
    }

    template<Real R>
    constexpr float Vector3<R>::LengthSquared(const Vector3& vec)
    {
        return vec.x * vec.x + vec.y * vec.y + vec.z * vec.z;
    }

    template<Real R>
    constexpr float Vector3<R>::Length(const Vector3& vec)
    {
        return Math::Sqrt(LengthSquared(vec));
    }

    template<Real R>
    constexpr Vector3<R> Vector3<R>::Normalize(const Vector3& vec)
    {
        auto l = Length(vec);
        return Vector3(vec.x / l, vec.y / l, vec.z / l);
    }

    template<Real R>
    constexpr Vector3<R> Vector3<R>::Cross(const Vector3& v1, const Vector3& v2)
    {
        return Vector3((v1.y * v2.z - v1.z * v2.y), 0, 0) - Vector3(0, (v1.x * v2.z - v1.z * v2.x), 0) -
               Vector3(0, 0, (v1.x * v2.y - v1.y * v2.x));
    }

    template<Real R>
    constexpr auto Vector3<R>::Dot(const Vector3& v1, const Vector3& v2)
    {
        return v1.x * v2.x + v1.y * v2.y + v1.z * v2.z;
    }

    template<Real R>
    Vector3<R> Vector3<R>::operator-() const
    {
        return Vector3{-x, -y, -z};
    }

    template<Real R>
    R Vector3<R>::Dot(const Vector3& vec) const
    {
        return vec.x * x + vec.y * y + vec.z * z;
    }

    template<Real R>
    Vector3<R> Vector3<R>::operator*(const R& t) const
    {
        return Vector3{x * t, y * t, z * t};
    }

    template<Real R>
    Vector3<R> Vector3<R>::operator+(const R& t) const
    {
        return Vector3{x + t, y + t, z + t};
    }

    template<Real R>
    Vector3<R> Vector3<R>::operator-(const R& t) const
    {
        return Vector3{x - t, y - t, z - t};
    }

    template<Real R>
    Vector3<R> Vector3<R>::operator*(const Vector3& vec) const
    {
        return Vector3{vec.x * x, vec.y * y, vec.z * z};
    }

    template<Real R>
    Vector3<R> Vector3<R>::operator+(const Vector3& vec) const
    {
        return Vector3{vec.x + x, vec.y + y, vec.z + z};
    }

    template<Real R>
    Vector3<R> Vector3<R>::operator-(const Vector3& vec) const
    {
        return Vector3{vec.x - x, vec.y - y, vec.z - z};
    }

    template<Real R>
    Vector3<R>& Vector3<R>::operator+=(const Vector3& vec)
    {
        x += vec.x;
        y += vec.y;
        z += vec.z;
        return *this;
    }

    template<Real R>
    Vector3<R>& Vector3<R>::operator-=(const Vector3& vec)
    {
        x -= vec.x;
        y -= vec.y;
        z -= vec.z;
        return *this;
    }

    template<Real R>
    Vector3<R>& Vector3<R>::operator*=(const Vector3& vec)
    {
        x *= vec.x;
        y *= vec.y;
        z *= vec.z;
        return *this;
    }

    template<Real R>
    Vector3<R>& Vector3<R>::operator+=(const R& t)
    {
        x += t;
        y += t;
        z += t;
        return *this;
    }

    template<Real R>
    Vector3<R>& Vector3<R>::operator-=(const R& t)
    {
        x -= t;
        y -= t;
        z -= t;
        return *this;
    }

    template<Real R>
    Vector3<R>& Vector3<R>::operator*=(const R& t)
    {
        x *= t;
        y *= t;
        z *= t;
        return *this;
    }

    template<Real R>
    Vector3<R>& Vector3<R>::operator/=(const R& t)
    {
        x /= t;
        y /= t;
        z /= t;
        return *this;
    }

}// namespace Wl
