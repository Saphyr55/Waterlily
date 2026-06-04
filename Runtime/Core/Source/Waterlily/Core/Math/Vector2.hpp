#pragma once

#include "Waterlily/Core/Defines.hpp"
#include "Waterlily/Core/Math/Math.hpp"

namespace Wl
{

    template<Real R = float>
    struct Vector2
    {
        R x = 0;
        R y = 0;

        R Dot(const Vector2& vec) const;

        Vector2 operator*(const Real auto&) const;
        Vector2 operator+(const Real auto&) const;
        Vector2 operator-(const Real auto&) const;
        Vector2 operator*(const Vector2&) const;
        Vector2 operator+(const Vector2&) const;
        Vector2 operator-(const Vector2&) const;
        Vector2 operator-() const;

        bool operator==(const Vector2& other) const
        {
            return x == other.x && y == other.y;
        }

        bool operator!=(const Vector2& other) const
        {
            return !(*this == other);
        }
    };

    using Vector2f = Vector2<float>;
    using Vector2d = Vector2<double>;
    using Vector2i = Vector2<int32_t>;
    using Vector2u = Vector2<uint32_t>;

    template<Real R>
    R Vector2<R>::Dot(const Vector2& vec) const
    {
        return vec.x * x + vec.y * y;
    }

    template<Real R>
    Vector2<R> Vector2<R>::operator*(const Real auto& t) const
    {
        return Vector2{x * t, y * t};
    }

    template<Real R>
    Vector2<R> Vector2<R>::operator+(const Real auto& t) const
    {
        return Vector2{x + t, y + t};
    }

    template<Real R>
    Vector2<R> Vector2<R>::operator-() const
    {
        return Vector2{-x, -y};
    }

    template<Real R>
    Vector2<R> Vector2<R>::operator-(const Real auto& t) const
    {
        return Vector2{x - t, y - t};
    }

    template<Real R>
    Vector2<R> Vector2<R>::operator*(const Vector2& vec) const
    {
        return Vector2{vec.x * x, vec.y * y};
    }

    template<Real R>
    Vector2<R> Vector2<R>::operator+(const Vector2& vec) const
    {
        return Vector2{vec.x + x, vec.y + y};
    }

    template<Real R>
    Vector2<R> Vector2<R>::operator-(const Vector2& vec) const
    {
        return Vector2{vec.x - x, vec.y - y};
    }

}// namespace Wl
