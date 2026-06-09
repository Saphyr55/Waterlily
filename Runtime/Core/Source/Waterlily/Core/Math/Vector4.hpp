#pragma once

#include "Waterlily/Core/Asserts.hpp"
#include "Waterlily/Core/IO/Stream.hpp"
#include "Waterlily/Core/Math/Vector3.hpp"

namespace Wl
{

    template<Real R = float>
    struct Vector4
    {
        R x = 0;
        R y = 0;
        R z = 0;
        R w = 0;

        constexpr Vector4() = default;
        constexpr explicit Vector4(R r);
        constexpr explicit Vector4(Vector3<R> vec, R w = 1);
        constexpr Vector4(R x, R y, R z, R w);

        constexpr R Dot(const Vector4& vec) const;
        constexpr const R& operator[](size_t i) const;
        constexpr R& operator[](size_t i);

        constexpr Vector4 operator*(const Real auto&) const;
        constexpr Vector4 operator+(const Real auto&) const;
        constexpr Vector4 operator-(const Real auto&) const;
        constexpr Vector4 operator/(const Real auto&) const;
        constexpr Vector4 operator*(const Vector4& vec) const;
        constexpr Vector4 operator/(const Vector4& vec) const;
        constexpr Vector4 operator+(const Vector4& vec) const;
        constexpr Vector4 operator-(const Vector4& vec) const;
        constexpr Vector4 operator-() const;

        Vector4& operator+=(const Vector4& vec);
        Vector4& operator-=(const Vector4& vec);
        Vector4& operator*=(const Vector4& vec);
        Vector4& operator/=(const Vector4& vec);
        Vector4& operator+=(const Real auto& t);
        Vector4& operator-=(const Real auto& t);
        Vector4& operator*=(const Real auto& t);
        Vector4& operator/=(const Real auto& t);

        bool operator==(const Vector4& other) const = default;
        bool operator!=(const Vector4& other) const = default;

        static constexpr float Length(Vector4 vec);
        static constexpr Vector4<float> Normalize(Vector4 vec);
    };

    template<typename RealType>
    inline void operator<<(OutputStream& stream, const Vector4<RealType>& v)
    {
        stream.Write(reinterpret_cast<const uint8_t*>(&v), sizeof(Vector4<RealType>));
    }

    template<typename RealType>
    inline void operator>>(InputStream& stream, Vector4<RealType>& v)
    {
        stream.Read(reinterpret_cast<uint8_t*>(&v), sizeof(Vector4<RealType>));
    }

    using Vector4f = Vector4<float>;
    using Vector4f32 = Vector4<float>;
    using Vector4f64 = Vector4<double>;
    using Vector4i = Vector4<int32_t>;
    using Vector4u = Vector4<uint32_t>;

    template<Real R>
    constexpr Vector4<R>::Vector4(Vector3<R> vec, R w)
        : Vector4(vec.x, vec.y, vec.z, w)
    {
    }

    template<Real R>
    constexpr Vector4<R>::Vector4(R x, R y, R z, R w)
        : x(x)
        , y(y)
        , z(z)
        , w(w)
    {
    }

    template<Real R>
    constexpr Vector4<R>::Vector4(R r)
        : Vector4(r, r, r, r)
    {
    }

    template<Real R>
    constexpr R Vector4<R>::Dot(const Vector4& vec) const
    {
        return vec.x * x + vec.y * y + vec.z * z + vec.w * w;
    }

    template<Real R>
    constexpr float Vector4<R>::Length(Vector4 vec)
    {
        return Math::Sqrt(vec.x * vec.x + vec.y * vec.y + vec.z * vec.z + vec.w * vec.w);
    }

    template<Real R>
    constexpr Vector4f Vector4<R>::Normalize(Vector4 vec)
    {
        float l = Length(vec);
        return Vector4f {vec.x / l, vec.y / l, vec.z / l, vec.w / l};
    }

    template<Real R>
    constexpr R& Vector4<R>::operator[](const size_t i)
    {
        WL_CHECK(i < 4);
        switch (i)
        {
            case 0:
                return x;
            case 1:
                return y;
            case 2:
                return z;
            case 3:
                return w;
            default:
                return x;
        }
        return x;
    }

    template<Real R>
    constexpr const R& Vector4<R>::operator[](size_t i) const
    {
        WL_CHECK(i < 4);
        switch (i)
        {
            case 0:
                return x;
            case 1:
                return y;
            case 2:
                return z;
            case 3:
                return w;
            default:
                return x;
        }
        return x;
    }

    template<Real R>
    constexpr Vector4<R> Vector4<R>::operator*(const Real auto& t) const
    {
        return Vector4 {x * t, y * t, z * t, w * t};
    }

    template<Real R>
    constexpr Vector4<R> Vector4<R>::operator+(const Real auto& t) const
    {
        return Vector4 {x + t, y + t, z + t, w + t};
    }

    template<Real R>
    constexpr Vector4<R> Vector4<R>::operator-(const Real auto& t) const
    {
        return Vector4 {x - t, y - t, z - t, w - t};
    }

    template<Real R>
    constexpr Vector4<R> Vector4<R>::operator/(const Real auto& t) const
    {
        return Vector4 {x / t, y / t, z / t, w / t};
    }

    template<Real R>
    constexpr Vector4<R> Vector4<R>::operator-() const
    {
        return Vector4 {-x, -y, -z, -w};
    }

    template<Real R>
    constexpr Vector4<R> Vector4<R>::operator*(const Vector4& vec) const
    {
        return Vector4 {x * vec.x, y * vec.y, z * vec.z, w * vec.w};
    }

    template<Real R>
    constexpr Vector4<R> Vector4<R>::operator/(const Vector4& vec) const
    {
        return Vector4 {x / vec.x, y / vec.y, z / vec.z, w / vec.w};
    }

    template<Real R>
    constexpr Vector4<R> Vector4<R>::operator+(const Vector4& vec) const
    {
        return Vector4 {x + vec.x, y + vec.y, z + vec.z, w + vec.w};
    }

    template<Real R>
    constexpr Vector4<R> Vector4<R>::operator-(const Vector4& vec) const
    {
        return Vector4 {x - vec.x, y - vec.y, z - vec.z, w - vec.w};
    }

    template<Real R>
    Vector4<R>& Vector4<R>::operator+=(const Vector4& vec)
    {
        x += vec.x;
        y += vec.y;
        z += vec.z;
        w += vec.w;
        return *this;
    }

    template<Real R>
    Vector4<R>& Vector4<R>::operator-=(const Vector4& vec)
    {
        x -= vec.x;
        y -= vec.y;
        z -= vec.z;
        w -= vec.w;
        return *this;
    }

    template<Real R>
    Vector4<R>& Vector4<R>::operator*=(const Vector4& vec)
    {
        x *= vec.x;
        y *= vec.y;
        z *= vec.z;
        w *= vec.w;
        return *this;
    }

    template<Real R>
    Vector4<R>& Vector4<R>::operator/=(const Vector4& vec)
    {
        x /= vec.x;
        y /= vec.y;
        z /= vec.z;
        w /= vec.w;
        return *this;
    }

    template<Real R>
    Vector4<R>& Vector4<R>::operator+=(const Real auto& t)
    {
        x += t;
        y += t;
        z += t;
        w += t;
        return *this;
    }

    template<Real R>
    Vector4<R>& Vector4<R>::operator-=(const Real auto& t)
    {
        x -= t;
        y -= t;
        z -= t;
        w -= t;
        return *this;
    }

    template<Real R>
    Vector4<R>& Vector4<R>::operator*=(const Real auto& t)
    {
        x *= t;
        y *= t;
        z *= t;
        w *= t;
        return *this;
    }

    template<Real R>
    Vector4<R>& Vector4<R>::operator/=(const Real auto& t)
    {
        x /= t;
        y /= t;
        z /= t;
        w /= t;
        return *this;
    }

}// namespace Wl
