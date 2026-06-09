#pragma once

#include "Waterlily/Core/Defines.hpp"

#include <cfloat>
#include <cmath>

namespace Wl
{

    template<typename RealType>
    concept Real = std::is_floating_point_v<RealType> || std::is_integral_v<RealType>;

    class Math
    {
    public:
        static constexpr inline auto PI = 3.14159265358979323846264338327950288;

        static constexpr inline auto PolarCap() -> auto
        {
            return PI - FLT_EPSILON;
        }

        static constexpr inline auto Sqrt(const Real auto& real) -> auto
        {
            return std::sqrt(real);
        }

        static constexpr inline auto Cos(const Real auto& real) -> auto
        {
            return std::cos(real);
        }

        static constexpr inline auto Sin(const Real auto& real) -> auto
        {
            return std::sin(real);
        }

        static constexpr inline auto Tan(const Real auto& real) -> auto
        {
            return std::tan(real);
        }

        static constexpr inline auto Abs(const Real auto& real) -> auto
        {
            return std::abs(real);
        }

        static constexpr inline auto Radians(const Real auto& angle) -> auto
        {
            return angle * (PI / 180.0);
        }

        static constexpr inline auto Degrees(const Real auto& angle) -> auto
        {
            return angle * (180.0 / PI);
        }

        static constexpr inline auto Signum(const Real auto& r) -> auto
        {
            return r == decltype(r)(0) ? 0 : rabs(r) / r;
        }

        static constexpr inline auto Clamp(const auto& value, const auto& min, const auto& max) -> auto
        {
            return (value < min) ? min : (value > max) ? max
                                                       : value;
        }

        static constexpr inline auto Atan(const Real auto& value) -> auto
        {
            return std::atan(value);
        }

        static constexpr inline auto Atan2(const Real auto& value1, const Real auto& value2) -> auto
        {
            return std::atan2(value1, value2);
        }

        static constexpr inline auto Mod(const Real auto& value, const Real auto& degree) -> auto
        {
            return std::fmod(value, degree);
        }

        static constexpr inline auto Asin(const Real auto& value) -> auto
        {
            return std::asin(value);
        }

        static constexpr inline auto Floor(const Real auto& value) -> auto
        {
            return std::floor(value);
        }

        static constexpr inline auto Ceil(const Real auto& value) -> auto
        {
            return std::ceil(value);
        }

        static constexpr inline auto Log(const Real auto& value) -> auto
        {
            return std::log(value);
        }

        static constexpr inline auto Log2(const Real auto& value) -> auto
        {
            return std::log2(value);
        }
            
        static constexpr inline auto Exp(const Real auto& value) -> auto
        {
            return std::exp(value);
        }
            
        template<Real RealType>
        static constexpr inline auto Min(const RealType& value, const RealType& min) -> RealType
        {
            return std::min(value, min);
        }

        template<Real RealType>
        static constexpr inline auto Max(const RealType& value, const RealType& max) -> RealType
        {
            return std::max(value, max);
        }
    };

}// namespace Wl
