/*
* core.hpp
* core preprocessor macros, traits and includes.
*/
#pragma once

#if defined(__clang__) 
#define LLEC_COMPILER_CLANG
#define LLEC_CPP_VER __cplusplus
#elif defined(__GNUC__)
#define LLEC_COMPILER_GCC
#define LLEC_CPP_VER __cplusplus
#elif defined(_MSC_VER)
#define LLEC_CPP_VER _MSVC_LANG
#else
#error llec error: compiler not supported!
#endif

#if (LLEC_CPP_VER < 202002L)
#error llec error: minimum c++20 required!
#endif

#if (_WIN32 || _WIN64)
#define LLEC_PLATFORM_WINDOWS
#elif defined(__linux__)
#define LLEC_PLATFORM_LINUX
#elif defined(__APPLE__)
#define LLEC_PLATFORM_APPLE
#endif // _WIN32

#include <cstddef>
#include <cstring>
#include <memory>
#include <type_traits>
#include <utility>
#include <bit>
#include <limits>
#include <algorithm>

#include <concepts>

#ifdef LLEC_DEBUG_BUILD
#include <assert.h>
#define LLEC_ASSERT(x) assert(x)
#else
#define LLEC_ASSERT(x) static_cast<void>(0)
#endif // LLEC_DEBUG_BUILD

#define LLEC_NODISCARD [[nodiscard]]
#define LLEC_LIKELY [[likely]]
#define LLEC_UNLIKELY [[unlikely]]

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
            std::is_trivially_move_constructible_v<T> && std::is_trivially_copy_constructible_v<T> && std::is_trivially_destructible_v<T>;

    } // namespace traits

} // namespace llec
