/*
 * fast_math.hpp
 * collection of fast but unsafe math functions. why are they unsafe? just because it doesn't do any error/exception
 * handling like the standard implementations.
 */

#pragma once
#include "core/core.hpp"

namespace llec::fast_math
{
    /// @brief inverse sqrt, similar to quake's implementation
    /// The implementation is similar to Wikipedia's but with modifications
    /// @tparam T single or double precision floating-point
    /// @param number
    /// @return inverse sqrt of the number
    template <typename T>
    requires std::is_floating_point_v<T>
    LLEC_NODISCARD constexpr T rsqrt(T number) noexcept
    {
        static_assert(std::numeric_limits<T>::is_iec559);
        using integral_type = std::conditional_t<traits::is_f32_v<T>, u32, u64>;
        constexpr integral_type magic = traits::is_f32_v<T> ? 0x5F375A86u : 0x5FE6EB50C7B537A9ull;
        const T y = std::bit_cast<T>(magic - (std::bit_cast<integral_type>(number) >> 1));
        return y * (T(1.5) - (number * T(0.5) * y * y));
    }

    /// @brief sqrt
    /// @tparam T single or double precision floating-pont
    /// @param number
    /// @return sqrt of the number
    template <typename T>
    requires std::is_floating_point_v<T>
    LLEC_NODISCARD constexpr T sqrt(T number) noexcept
    {
        return number * rsqrt(number);
    }

    /// @brief truncate
    /// @tparam T single or double precision floating-point
    /// @param x
    /// @return integral part for a floatin-point number
    template <typename T>
    requires std::is_floating_point_v<T>
    LLEC_NODISCARD constexpr T trunc(T x) noexcept
    {
        return static_cast<std::conditional_t<traits::is_f32_v<T>, s32, s64>>(x);
    }

    /// @brief fmod
    /// @tparam T single or double precision floating-point
    /// @param x dividend (numerator)
    /// @param y divisor (denominator)
    /// @return modulo
    template <typename T>
    requires std::is_floating_point_v<T>
    LLEC_NODISCARD constexpr T fmod(T x, T y) noexcept
    {
        return x - trunc<T>(x / y) * y;
    }

    /// @brief signum
    /// @tparam T must be integral type
    /// @param x
    /// @return 1 for positive, -1 for negative, 0 for zero
    template <typename T>
    requires std::is_integral_v<T>
    LLEC_NODISCARD constexpr T signum(T x) noexcept
    {
        return static_cast<T>(static_cast<s32>(x > T(0)) - static_cast<s32>(x < T(0)));
    }

    /// @brief is_odd
    /// @tparam T must be integral type
    /// @param x
    /// @return true if odd else false
    template <typename T>
    requires std::is_integral_v<T>
    LLEC_NODISCARD constexpr bool is_odd(T x) noexcept
    {
        return (x & 1);
    }

    /// @brief is_even
    /// @tparam T must be integral type
    /// @param x
    /// @return true if even else false
    template <typename T>
    requires std::is_integral_v<T>
    LLEC_NODISCARD constexpr bool is_even(T x) noexcept
    {
        return !(x & 1);
    }
} // namespace llec::fast_math
