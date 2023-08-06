/*
 * pair.hpp
 * minimalistic pair structure
 */

#pragma once

namespace llec
{
    /// @brief minimalistic pair structure
    /// @tparam T1 type for the first member
    /// @tparam T2 type for the second member
    template <typename T1, typename T2>
    struct pair
    {
        T1 first;
        T2 second;
    };
} // namespace llec