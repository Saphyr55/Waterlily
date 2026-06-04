#pragma once

#include "Waterlily/Core/Math/Math.hpp"
#include "Waterlily/Core/Math/Vector3.hpp"

namespace Wl
{

    template<Real R>
    struct Matrix3
    {
        using Mat = Matrix3<R>;
        using Vec = Vector3<R>;

        Mat operator*(Mat m);
        Vec operator*(Vec v);
        Vec& operator[](size_t i);
        Vec col(uint32_t index) const;

        Matrix3() = default;
        Matrix3(R r);
        Matrix3(R m11, R m21, R m31, R m12, R m22, R m32, R m13, R m23, R m33);
        Matrix3(const Vec& col_1, const Vec& col_2, const Vec& col_3);

        static Matrix3 Identity();
        // static std::array<R, 3 * 3> ValueArray(Mat mat);
        static Matrix3 Translate(Mat mat, Vector3<R> vec);
        static Matrix3 Scale(Mat mat, Vector3<R> vec);
        static Matrix3 Rotate(Mat mat, const Real auto& theta, Vector3<R> vec);
        static Matrix3 RotateX(float theta);
        static Matrix3 RotateY(float theta);
        static Matrix3 RotateZ(float theta);

    private:
        Vec m_value[3];
    };

    using Matrix3f = Matrix3<float>;
    using Matrix3f32 = Matrix3<float>;
    using Matrix3f64 = Matrix3<double>;

    template<Real R>
    Matrix3<R> Matrix3<R>::Identity()
    {
        return Matrix3(1);
    }

    template<Real R>
    Matrix3<R> Matrix3<R>::Translate(Mat mat, Vector3<R> vec)
    {
        Mat transformationMat(1);
        transformationMat[2] = vec;
        return mat * transformationMat;
    }

    template<Real R>
    Matrix3<R> Matrix3<R>::Scale(Mat mat, Vector3<R> vec)
    {
        return mat * Mat(Vector3<R>(vec.x, 0, 0), Vector3<R>(0, vec.y, 0), Vector4<R>(0, 0, vec.z));
    }

    template<Real R>
    Matrix3<R> Matrix3<R>::Rotate(Mat mat, const Real auto& theta, Vector3<R> vec)
    {
        auto c = Math::Cos(theta);
        auto s = Math::Sin(theta);
        auto v = Vector3<R>::Normalize(vec);

        return mat * Mat(c + v.x * v.x * (1 - c),
                         v.x * v.y * (1 - c) - v.z * s,
                         v.x * v.z * (1 - c) + v.y * s,
                         v.x * v.y * (1 - c) + v.z * s,
                         c + v.y * v.y * (1 - c),
                         v.y * v.z * (1 - c) - v.x * s,
                         v.x * v.z * (1 - c) - v.y * s,
                         v.z * v.y * (1 - c) + v.x * s,
                         c + v.z * v.z * (1 - c));
    }

    template<Real R>
    Matrix3<R> Matrix3<R>::RotateX(const float theta)
    {
        return Mat(Vector3<R>(1, 0, 0),
                   Vector3<R>(0, Math::Cos(theta), Math::Sin(theta)),
                   Vector3<R>(0, -Math::Sin(theta), Math::Cos(theta)));
    }

    template<Real R>
    Matrix3<R> Matrix3<R>::RotateY(const float theta)
    {
        return Mat(Vector3<R>(Math::Cos(theta), 0, -Math::Sin(theta)),
                   Vector3<R>(0, 1, 0),
                   Vector3<R>(Math::Sin(theta), 0, Math::Cos(theta)));
    }

    template<Real R>
    Matrix3<R> Matrix3<R>::RotateZ(const float theta)
    {
        return Matrix3<R>(Vector3<R>(Math::Cos(theta), Math::Sin(theta), 0),
                          Vector3<R>(-Math::Sin(theta), Math::Cos(theta), 0),
                          Vector3<R>(0, 0, 1));
    }

    template<Real R>
    Matrix3<R>::Vec Matrix3<R>::col(uint32_t index) const
    {
        return (*this)[index];
    }

    template<Real R>
    Matrix3<R>::Matrix3(const Vec& col1, const Vec& col2, const Vec& col3)
    {
        m_value[0] = col1;
        m_value[1] = col2;
        m_value[2] = col3;
    }

    template<Real R>
    Matrix3<R>::Matrix3(R m11, R m21, R m31, R m12, R m22, R m32, R m13, R m23, R m33)
        : Matrix3(Vec(m11, m12, m13), Vec(m21, m22, m23), Vec(m31, m32, m33))
    {
    }

    template<Real R>
    Matrix3<R>::Matrix3(R r)
        : Matrix3(Vec(r, 0, 0), Vec(0, r, 0), Vec(0, 0, r))
    {
    }

    template<Real R>
    auto Matrix3<R>::operator*(Mat m) -> Mat
    {
        Mat result(0);
        for (int8_t i = 0; i < 3; i++)
        {
            for (int8_t j = 0; j < 3; j++)
            {
                R rji{};
                for (int8_t k = 0; k < 3; k++)
                {
                    rji += m[j][k] * m_value[k][i];
                }
                result[j][i] = rji;
            }
        }
        return result;
    }

    template<Real R>
    Vector3<R> Matrix3<R>::operator*(Vector3<R> v)
    {
        return Vec((*this)[0][0] * v[0] + (*this)[0][1] * v[1] + (*this)[0][2] * v[2],
                   (*this)[1][0] * v[0] + (*this)[1][1] * v[1] + (*this)[1][2] * v[2],
                   (*this)[2][0] * v[0] + (*this)[2][1] * v[1] + (*this)[2][2] * v[2]);
    }

    template<Real R>
    Vector3<R>& Matrix3<R>::operator[](size_t i)
    {
        return m_value[i];
    }

}// namespace Wl
