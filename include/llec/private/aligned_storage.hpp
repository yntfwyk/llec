/*
 * aligned_storage.hpp
 * similar to std::aligned_storage which has been deprecated in c++23.
 */

#pragma once
#include "../core/core.hpp"

namespace llec
{
    namespace details
    {
        template <std::size_t Size, std::size_t Alignment>
        struct aligned_storage
        {
            struct type
            {
                alignas(Alignment) std::byte buffer[Size];
            };
        };

        template <std::size_t Size, std::size_t Alignment>
        using aligned_storage_t = typename aligned_storage<Size, Alignment>::type;

    } // namespace details
} // namespace llec