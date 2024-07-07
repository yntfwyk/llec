/*
 * checksum.hpp
 * collection light-weight checksum functions.
 */

#pragma once
#include "core/core.hpp"
#include <string_view>

namespace llec::checksum
{
    /// @brief adler32
    /// @param data to get a checksum of
    /// @param length in bytes
    /// @return checksum of type u32
    LLEC_NODISCARD constexpr u32 adler32(const void* data, std::size_t length) noexcept
    {
        constexpr u32 ADLER_MAGIC = 65521u;
        const u8* bytes = static_cast<const u8*>(data);
        u32 a = 1u, b = 0u;
        for (std::size_t i = 0u; i < length; i++)
        {
            a = (a + bytes[i]) % ADLER_MAGIC;
            b = (b + a) % ADLER_MAGIC;
        }
        return (b << 16u) | a;
    }

    /// @brief adler32
    /// @param compile-time string to get a checksum of
    /// @return checksum of type u32
    LLEC_NODISCARD constexpr u32 adler32(std::string_view str) noexcept
    {
        constexpr u32 ADLER_MAGIC = 65521u;
        const std::size_t length = str.length();
        u32 a = 1u, b = 0u;
        for (std::size_t i = 0u; i < length; i++)
        {
            a = (a + str[i]) % ADLER_MAGIC;
            b = (b + a) % ADLER_MAGIC;
        }
        return (b << 16u) | a;
    }

    /// @brief fnv1a (32 or 64)
    /// @tparam T u32 or u64
    /// @param data to get a hash of
    /// @param length in bytes
    /// @return hash
    template <typename T = u32>
    requires traits::u32_64_integral<T>
    LLEC_NODISCARD constexpr T fnv1a(const void* data, std::size_t length) noexcept
    {
        T hash = traits::is_u64_v<T> ? 0xCBF29CE484222325u : 0x811C9DC5u;
        constexpr T fnv_prime = traits::is_u64_v<T> ? 0x100000001B3u : 0x01000193u;

        const u8* bytes = static_cast<const u8*>(data);
        for (std::size_t i = 0u; i < length; i++)
        {
            hash = hash ^ bytes[i];
            hash = hash * fnv_prime;
        }
        return hash;
    }

    /// @brief fnv1a (32 or 64) for compile-time strings
    /// @tparam T u32 or u64
    /// @param compile-time string to get a hash of
    /// @return hash
    template <typename T = u32>
    requires traits::u32_64_integral<T>
    LLEC_NODISCARD constexpr T fnv1a(std::string_view str) noexcept
    {
        T hash = traits::is_u64_v<T> ? 0xCBF29CE484222325u : 0x811C9DC5u;
        constexpr T fnv_prime = traits::is_u64_v<T> ? 0x100000001B3u : 0x01000193u;
        const std::size_t length = str.length();
        for (std::size_t i = 0u; i < length; i++)
        {
            hash = hash ^ str[i];
            hash = hash * fnv_prime;
        }
        return hash;
    }

} // namespace llec::checksum
namespace llec::literals
{
    constexpr u32 operator""_adler(const char* str, [[maybe_unused]] std::size_t len) noexcept
    {
        return checksum::adler32(str);
    }

    constexpr u32 operator""_fnv32(const char* str, [[maybe_unused]] std::size_t len) noexcept
    {
        return checksum::fnv1a(str);
    }

    constexpr u64 operator""_fnv64(const char* str, [[maybe_unused]] std::size_t len) noexcept
    {
        return checksum::fnv1a<u64>(str);
    }
} // namespace llec::literals
