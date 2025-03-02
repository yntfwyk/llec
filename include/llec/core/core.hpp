/*
 * core.hpp
 * core preprocessor macros, traits and includes.
 */

#pragma once

#define LLEC_VERSION_MAJOR 1
#define LLEC_VERSION_MINOR 0
#define LLEC_VEERSION_PATCH 0

#if defined(__clang__)
#define LLEC_COMPILER_CLANG
#define LLEC_CPP_VER __cplusplus
#elif defined(__GNUC__)
#define LLEC_COMPILER_GCC
#define LLEC_CPP_VER __cplusplus
#elif defined(_MSC_VER)
#define LLEC_COMPILER_MSVC
#define LLEC_CPP_VER _MSVC_LANG
#else
#error llec error: compiler not supported!
#endif

#if (LLEC_CPP_VER < 202002L)
#error llec error: minimum c++20 required!
#endif

#if (LLEC_CPP_VER == 202002L)
#define LLEC_CPP20
#else //(LLEC_CPP_VER == ???)
#define LLEC_CPP23
#endif

#if (_WIN32 || _WIN64)
#define LLEC_PLATFORM_WINDOWS
#elif defined(__linux__)
#define LLEC_PLATFORM_LINUX
#elif defined(__APPLE__)
#define LLEC_PLATFORM_APPLE
#endif // _WIN32

#include <algorithm>
#include <bit>
#include <cstddef>
#include <cstring>
#include <functional>
#include <limits>
#include <memory>
#include <type_traits>
#include <utility>

#include <concepts>

#ifdef LLEC_PLATFORM_WINDOWS
#include <Windows.h>
#endif

#ifdef LLEC_DEBUG_BUILD
#include <assert.h>
#define LLEC_ASSERT(x) assert(x)
#else
#define LLEC_ASSERT(x) static_cast<void>(0)
#endif // LLEC_DEBUG_BUILD

#define LLEC_NODISCARD [[nodiscard]]

namespace llec
{
    using s8 = signed char;
    using s16 = signed short;
    using s32 = signed int;
    using s64 = signed long long;

    using u8 = unsigned char;
    using u16 = unsigned short;
    using u32 = unsigned int;
    using u64 = unsigned long long;

    using f32 = float;
    using f64 = double;

    namespace traits
    {
        template <typename T>
        inline constexpr bool is_s8_v = std::is_same_v<T, s8>;

        template <typename T>
        inline constexpr bool is_u8_v = std::is_same_v<T, u8>;

        template <typename T>
        inline constexpr bool is_s16_v = std::is_same_v<T, s16>;

        template <typename T>
        inline constexpr bool is_u16_v = std::is_same_v<T, u16>;

        template <typename T>
        inline constexpr bool is_s32_v = std::is_same_v<T, s32>;

        template <typename T>
        inline constexpr bool is_u32_v = std::is_same_v<T, u32>;

        template <typename T>
        inline constexpr bool is_s64_v = std::is_same_v<T, s64>;

        template <typename T>
        inline constexpr bool is_u64_v = std::is_same_v<T, u64>;

        template <typename T>
        inline constexpr bool is_f32_v = std::is_same_v<T, f32>;

        template <typename T>
        inline constexpr bool is_f64_v = std::is_same_v<T, f64>;

        template <typename T>
        inline constexpr bool is_trivially_xstructible_v =
            std::is_trivially_constructible_v<T> && std::is_trivially_destructible_v<T>;

        template <typename T>
        concept single_precision = is_f32_v<T>;
        template <typename T>
        concept double_precision = is_f64_v<T>;
        template <typename T>
        concept u32_64_integral = is_u32_v<T> || is_u64_v<T>;
        template <typename T>
        concept trivially_relocatable =
            (std::is_trivially_move_constructible_v<T> && std::is_trivially_destructible_v<T>) ||
            std::is_trivially_copyable_v<T>;
        template <typename T>
        concept relocatable = std::is_move_constructible_v<T> && std::is_destructible_v<T>;

        // clang-format off
        template <typename T>
        concept is_addable = requires(T a, T b)
        {
            {a + b} -> std::same_as<T>;
        };
        
        template <typename T>
        concept is_subtractable = requires(T a, T b) 
        {
            {a - b} -> std::same_as<T>;
        };
        
        template <typename T>
        concept is_multiplyable = requires(T a, T b)
        {
            {a * b} -> std::same_as<T>;
        };
        
        template <typename T>
        concept is_divideable = requires(T a, T b)
        {
            {a / b} -> std::same_as<T>;
        };

        template <typename T>
        concept is_left_shiftable = requires(T a, T b) 
        {
            {a << b} -> std::same_as<T>;
        };

        template <typename T>
        concept is_right_shiftable = requires(T a, T b)
        {
            {a >> b} -> std::same_as<T>;
        };

        template <typename T>
        concept is_orable = requires(T a, T b)
        {
            {a | b} -> std::same_as<T>;
        };

        template <typename T>
        concept is_xorable = requires(T a, T b)
        {
            {a ^ b} -> std::same_as<T>;
        };

        template <typename T>
        concept is_add_assignable = requires(T a, T b)
        {
            {a += b} -> std::same_as<T&>;
        };

        template <typename T>
        concept is_subtract_assignable = requires(T a, T b)
        {
            {a -= b} -> std::same_as<T&>;
        };

        template <typename T>
        concept is_multiply_assignable = requires(T a, T b)
        {
            {a *= b} -> std::same_as<T&>;
        };

        template <typename T>
        concept is_divide_assignable = requires(T a, T b)
        {
            {a /= b} -> std::same_as<T&>;
        };

        template <typename T>
        concept is_left_shift_assignable = requires(T a, T b) 
        {
            {a <<= b} -> std::same_as<T&>;
        };

        template <typename T>
        concept is_right_shift_assignable = requires(T a, T b)
        {
            {a >>= b} -> std::same_as<T&>;
        };

        template <typename T>
        concept is_or_assignable = requires(T a, T b) 
        {
            {a |= b} -> std::same_as<T&>;
        };

        template <typename T>
        concept is_xor_assignable = requires(T a, T b) 
        {
            {a ^= b} -> std::same_as<T&>;
        };

        template <typename T>
        concept is_equal_comparable = requires(T a, T b)
        {
            {a == b} -> std::same_as<bool>;
        };

        template <typename T>
        concept is_notequal_comparable = requires(T a, T b)
        {
            {a != b} -> std::same_as<bool>;
        };

        template <typename T>
        concept is_lessthan_comparable = requires(T a, T b)
        {
            {a < b} -> std::same_as<bool>;
        };

        template <typename T>
        concept is_lessthan_equal_comparable = requires(T a, T b)
        {
            {a <= b} -> std::same_as<bool>;
        };

        template <typename T>
        concept is_greaterthan_comparable = requires(T a, T b)
        {
            {a > b}  -> std::same_as<bool>;
        };
        
        template <typename T>
        concept is_greaterthan_equal_comparable = requires(T a, T b) 
        {
            {a >= b} -> std::same_as<bool>;
        };
        
        template <typename T>
        concept is_spaceship_comparable = requires(T a, T b)
        {
            {a <=> b};
        };

        template <typename T>
        concept is_vector_like = requires(T a, typename T::size_type n, const typename T::value_type& v, typename T::iterator it)
        {
            typename T::value_type;
            typename T::iterator;
            typename T::const_iterator;
            {a.begin()} -> std::same_as<typename T::iterator>;
            {a.end()} -> std::same_as<typename T::iterator>;
            {a.cbegin()} -> std::same_as<typename T::const_iterator>;
            {a.cend()} -> std::same_as<typename T::const_iterator>;
            {a[n]} -> std::same_as<typename T::value_type&>;
            {a.push_back(v)};
            {a.erase(it)};
            {a.size()};
            {a.data()};
        } && std::contiguous_iterator<typename T::iterator> && std::contiguous_iterator<typename T::const_iterator>;

        // clang-format on
    } // namespace traits

} // namespace llec
