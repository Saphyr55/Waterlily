#pragma once

#include "Waterlily/Core/Defines.hpp"
#include "Waterlily/Core/Math/Math.hpp"
#include "Waterlily/Core/Math/Vector3.hpp"
#include "Waterlily/Core/Math/Vector4.hpp"

namespace Wl
{

    template<Real R = float>
    struct Matrix4
    {
        using Mat = Matrix4<R>;
        using Vec = Vector4<R>;

        constexpr Mat operator*(const Mat& m) const;

        constexpr Vec operator*(const Vector4<R>& v) const;

        constexpr Vec& operator[](size_t i);

        constexpr const Vec& operator[](size_t i) const;

        constexpr Vec Col(uint32_t index) const;

        constexpr Matrix4() = default;

        constexpr Matrix4(R r);

        constexpr Matrix4(R m11,
                          R m21,
                          R m31,
                          R m41,
                          R m12,
                          R m22,
                          R m32,
                          R m42,
                          R m13,
                          R m23,
                          R m33,
                          R m43,
                          R m14,
                          R m24,
                          R m34,
                          R m44);

        constexpr Matrix4(const Vec& col_1, const Vec& col_2, const Vec& col_3, const Vec& col_4);

        static constexpr Matrix4 Identity();

        static constexpr Matrix4 Translate(const Mat& mat, const Vector3<R>& vec);

        static constexpr Matrix4 Scale(const Mat& mat, const Vector3<R>& vec);

        static constexpr Matrix4 Rotate(const Mat& mat, const Real auto& theta, const Vector3<R>& vec);

        static constexpr Matrix4 RotateX(const float& theta);

        static constexpr Matrix4 RotateY(const float& theta);

        static constexpr Matrix4 RotateZ(const float& theta);

        static constexpr Matrix4 Perspective(const Real auto& view,
                                             const Real auto& aspect,
                                             const Real auto& near,
                                             const Real auto& far);

        static constexpr Matrix4 Orthographic(const Real auto& left,
                                              const Real auto& right,
                                              const Real auto& bottom,
                                              const Real auto& top,
                                              const Real auto& near,
                                              const Real auto& far);

        static constexpr Matrix4 LookAt(const Vector3<R>& eye, const Vector3<R>& center, const Vector3<R>& up);

    private:
        Vec m_value[4];
    };

    template<Real RealType>
    inline void operator<<(OutputStream& stream, const Matrix4<RealType>& v)
    {
        stream.Write(reinterpret_cast<const uint8_t*>(v), sizeof(Matrix4<RealType>));
    }

    template<Real RealType>
    inline void operator>>(InputStream& stream, Matrix4<RealType>& v)
    {
        stream.Read(reinterpret_cast<uint8_t*>(v), sizeof(Matrix4<RealType>));
    }

    using Matrix4f = Matrix4<float>;
    using Matrix4d = Matrix4<double>;

    template<Real R>
    constexpr Matrix4<R> Matrix4<R>::Identity()
    {
        return Matrix4(1);
    }

    template<Real R>
    constexpr Matrix4<R> Matrix4<R>::Translate(const Mat& mat, const Vector3<R>& vec)
    {
        Mat transformation(1);
        transformation[3] = Vector4<R>(vec, 1);
        return mat * transformation;
    }

    template<Real R>
    constexpr Matrix4<R> Matrix4<R>::Scale(const Mat& mat, const Vector3<R>& vec)
    {
        return mat * Mat(Vector4<R>(vec.x, 0, 0, 0),
                         Vector4<R>(0, vec.y, 0, 0),
                         Vector4<R>(0, 0, vec.z, 0),
                         Vector4<R>(0, 0, 0, 1));
    }

    template<Real R>
    constexpr Matrix4<R> Matrix4<R>::Rotate(const Mat& mat, const Real auto& theta, const Vector3<R>& vec)
    {
        auto c = Math::Cos(theta);
        auto s = Math::Sin(theta);
        auto v = Vector3<R>::Normalize(vec);

        return mat * Mat(c + v.x * v.x * (1 - c),
                         v.x * v.y * (1 - c) - v.z * s,
                         v.x * v.z * (1 - c) + v.y * s,
                         0,
                         v.x * v.y * (1 - c) + v.z * s,
                         c + v.y * v.y * (1 - c),
                         v.y * v.z * (1 - c) - v.x * s,
                         0,
                         v.x * v.z * (1 - c) - v.y * s,
                         v.z * v.y * (1 - c) + v.x * s,
                         c + v.z * v.z * (1 - c),
                         0,
                         0,
                         0,
                         0,
                         1);
    }

    template<Real R>
    constexpr Matrix4<R> Matrix4<R>::RotateX(const float& theta)
    {
        return Mat(Vector4<R>(1, 0, 0, 0),
                   Vector4<R>(0, Math::Cos(theta), Math::Sin(theta), 0),
                   Vector4<R>(0, -Math::Sin(theta), Math::Cos(theta), 0),
                   Vector4<R>(0, 0, 0, 1));
    }

    template<Real R>
    constexpr Matrix4<R> Matrix4<R>::RotateY(const float& theta)
    {
        return Mat(Vector4<R>(Math::Cos(theta), 0, -Math::Sin(theta), 0),
                   Vector4<R>(0, 1, 0, 0),
                   Vector4<R>(Math::Sin(theta), 0, Math::Cos(theta), 0),
                   Vector4<R>(0, 0, 0, 1));
    }

    template<Real R>
    constexpr Matrix4<R> Matrix4<R>::RotateZ(const float& theta)
    {
        return Matrix4<R>(Vector4<R>(Math::Cos(theta), Math::Sin(theta), 0, 0),
                          Vector4<R>(-Math::Sin(theta), Math::Cos(theta), 0, 0),
                          Vector4<R>(0, 0, 1, 0),
                          Vector4<R>(0, 0, 0, 1));
    }

    template<Real R>
    constexpr Matrix4<R> Matrix4<R>::Perspective(const Real auto& view,
                                                 const Real auto& aspect,
                                                 const Real auto& near_,
                                                 const Real auto& far_)
    {
        Mat result(0.0);

        const R tanHalfFovy = Math::Tan(view / static_cast<R>(2.0));

        result[0][0] = R(1.0) / R(tanHalfFovy * aspect);
        result[1][1] = R(1.0) / tanHalfFovy;
        result[2][2] = R(-far_ - near_) / R(far_ - near_);
        result[3][2] = -R(2.0 * near_ * far_) / R(far_ - near_);
        result[2][3] = -R(1.0);
        return result;
    }

    template<Real R>
    constexpr Matrix4<R> Matrix4<R>::Orthographic(const Real auto& left,
                                                  const Real auto& right,
                                                  const Real auto& bottom,
                                                  const Real auto& top,
                                                  const Real auto& pnear,
                                                  const Real auto& pfar)
    {
        WL_CHECK(pfar - pnear != 0);
        Mat result(1.0);
        result[0][0] = R(2.0f) / R(right - left);
        result[1][1] = R(2.0f) / R(top - bottom);
        result[2][2] = -R(2.0f) / R(pfar - pnear);
        result[3][0] = -R(right + left) / R(right - left);
        result[3][1] = -R(top + bottom) / R(top - bottom);
        result[3][2] = -R(pfar + pnear) / R(pfar - pnear);
        return result;
    }

    template<Real R>
    constexpr Matrix4<R> Matrix4<R>::LookAt(const Vector3<R>& eye, const Vector3<R>& center, const Vector3<R>& up)
    {
        auto f = Vector3<R>::Normalize(center - eye);
        auto s = Vector3<R>::Normalize(Vector3<R>::Cross(f, up));
        auto u = Vector3<R>::Cross(s, f);

        Mat result(1.0);
        result[0][0] = s.x;
        result[1][0] = s.y;
        result[2][0] = s.z;
        result[0][1] = u.x;
        result[1][1] = u.y;
        result[2][1] = u.z;
        result[0][2] = f.x;
        result[1][2] = f.y;
        result[2][2] = f.z;
        result[3][0] = -s.Dot(eye);
        result[3][1] = -u.Dot(eye);
        result[3][2] = -f.Dot(eye);
        return result;
    }

    template<Real R>
    constexpr Vector4<R> Matrix4<R>::Col(uint32_t index) const
    {
        return (*this)[index];
    }

    template<Real R>
    constexpr Matrix4<R>::Matrix4(const Vec& col1, const Vec& col2, const Vec& col3, const Vec& col4)
    {
        m_value[0] = col1;
        m_value[1] = col2;
        m_value[2] = col3;
        m_value[3] = col4;
    }

    template<Real R>
    constexpr Matrix4<R>::Matrix4(R m11,
                                  R m21,
                                  R m31,
                                  R m41,
                                  R m12,
                                  R m22,
                                  R m32,
                                  R m42,
                                  R m13,
                                  R m23,
                                  R m33,
                                  R m43,
                                  R m14,
                                  R m24,
                                  R m34,
                                  R m44)
        : Matrix4(Vec(m11, m12, m13, m14), Vec(m21, m22, m23, m24), Vec(m31, m32, m33, m34), Vec(m41, m42, m43, m44))
    {
    }

    template<Real R>
    constexpr Matrix4<R>::Matrix4(R r)
        : Matrix4(Vec(r, 0, 0, 0), Vec(0, r, 0, 0), Vec(0, 0, r, 0), Vec(0, 0, 0, r))
    {
    }

    template<Real R>
    constexpr Matrix4<R> Matrix4<R>::operator*(const Mat& m) const
    {
        Mat result(0);
        for (int8_t i = 0; i < 4; i++)
        {
            for (int8_t j = 0; j < 4; j++)
            {
                R r_ji{};
                for (int8_t k = 0; k < 4; k++)
                {
                    r_ji += m[j][k] * m_value[k][i];
                }
                result[j][i] = r_ji;
            }
        }
        return result;
    }

    template<Real R>
    constexpr Vector4<R> Matrix4<R>::operator*(const Vector4<R>& v) const
    {
        return Vec((*this)[0][0] * v[0] + (*this)[0][1] * v[1] + (*this)[0][2] * v[2] + (*this)[0][3] * v[3],
                   (*this)[1][0] * v[0] + (*this)[1][1] * v[1] + (*this)[1][2] * v[2] + (*this)[1][3] * v[3],
                   (*this)[2][0] * v[0] + (*this)[2][1] * v[1] + (*this)[2][2] * v[2] + (*this)[2][3] * v[3],
                   (*this)[3][0] * v[0] + (*this)[3][1] * v[1] + (*this)[3][2] * v[2] + (*this)[3][3] * v[3]);
    }

    template<Real R>
    constexpr const Vector4<R>& Matrix4<R>::operator[](size_t i) const
    {
        return m_value[i];
    }

    template<Real R>
    constexpr Vector4<R>& Matrix4<R>::operator[](size_t i)
    {
        return m_value[i];
    }

}// namespace Wl
