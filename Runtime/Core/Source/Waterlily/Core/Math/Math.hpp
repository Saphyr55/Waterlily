#pragma once

#include <algorithm>
#include <cfloat>
#include <cmath>

namespace Wl
{
    using Real = float;

    template<typename RealType>
    concept IsReal = std::is_floating_point_v<RealType> || std::is_integral_v<RealType>;

    class Math
    {
    public:
        static constexpr inline auto PI = 3.14159265358979323846264338327950288;

        static constexpr inline auto PolarCap() -> auto
        {
            return PI - FLT_EPSILON;
        }

        static constexpr inline auto Sqrt(const IsReal auto& real) -> auto
        {
            return std::sqrt(real);
        }

        static constexpr inline auto Cos(const IsReal auto& real) -> auto
        {
            return std::cos(real);
        }

        static constexpr inline auto Sin(const IsReal auto& real) -> auto
        {
            return std::sin(real);
        }

        static constexpr inline auto Tan(const IsReal auto& real) -> auto
        {
            return std::tan(real);
        }

        static constexpr inline auto Abs(const IsReal auto& real) -> auto
        {
            return std::abs(real);
        }

        static constexpr inline auto Radians(const IsReal auto& angle) -> auto
        {
            return angle * (PI / 180.0);
        }

        static constexpr inline auto Degrees(const IsReal auto& angle) -> auto
        {
            return angle * (180.0 / PI);
        }

        static constexpr inline auto Signum(const IsReal auto& r) -> auto
        {
            return r == decltype(r)(0) ? 0 : rabs(r) / r;
        }

        static constexpr inline auto Clamp(const auto& value, const auto& min, const auto& max) -> auto
        {
            return (value < min) ? min : (value > max) ? max
                                                       : value;
        }

        static constexpr inline auto Atan(const IsReal auto& value) -> auto
        {
            return std::atan(value);
        }

        static constexpr inline auto Atan2(const IsReal auto& value1, const IsReal auto& value2) -> auto
        {
            return std::atan2(value1, value2);
        }

        static constexpr inline auto Mod(const IsReal auto& value, const IsReal auto& degree) -> auto
        {
            return std::fmod(value, degree);
        }

        static constexpr inline auto Asin(const IsReal auto& value) -> auto
        {
            return std::asin(value);
        }

        static constexpr inline auto Floor(const IsReal auto& value) -> auto
        {
            return std::floor(value);
        }

        static constexpr inline auto Ceil(const IsReal auto& value) -> auto
        {
            return std::ceil(value);
        }

        static constexpr inline auto Log(const IsReal auto& value) -> auto
        {
            return std::log(value);
        }

        static constexpr inline auto Log2(const IsReal auto& value) -> auto
        {
            return std::log2(value);
        }
            
        static constexpr inline auto Exp(const IsReal auto& value) -> auto
        {
            return std::exp(value);
        }
            
        template<IsReal RealType>
        static constexpr inline auto Min(const RealType& value, const RealType& min) -> RealType
        {
            return std::min(value, min);
        }

        template<IsReal RealType>
        static constexpr inline auto Max(const RealType& value, const RealType& max) -> RealType
        {
            return std::max(value, max);
        }
    };

}// namespace Wl
