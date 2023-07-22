/*
 * checksum.hpp
 * collection light-weight checksum functions.
 */

#pragma once
#include "../core/core.hpp"

namespace llec::checksum
{
    /// @brief adler32
    /// @param data to get a checksum of
    /// @param length in bytes
    /// @return checksum of type u32
    constexpr u32 adler32(const void* data, std::size_t length)
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

    /// @brief fnv1a (32 or 64)
    /// @tparam T u32 or u64
    /// @param data to get a hash of
    /// @param length in bytes
    /// @return hash
    template <typename T = u32>
        requires traits::u32_64_integral<T>
    constexpr T fnv1a(const void* data, std::size_t length)
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

} // namespace llec::checksum
