/*
 * typed_handle.hpp
 * a type safe wrapper class around a type
 */

#pragma once
#include "core/core.hpp"
#include "core/memory.hpp"

namespace llec
{
    /// @brief type safe wrapper around a type
    /// @tparam Tag unique tag to distinguish between types
    /// @tparam RawType underlying type
    template <typename RawType, typename Tag>
    class typed_handle
    {
      public:
        using raw_type = RawType;
        using tag_type = Tag;
        using value_type = raw_type;
        using reference = value_type&;
        using const_reference = const value_type&;

        typed_handle() noexcept = default;
        constexpr typed_handle(const_reference data) noexcept
        {
            m_typedData = data;
        }

        /// @brief gets the underlying data
        /// @return copy of the data
        LLEC_NODISCARD constexpr raw_type data() const noexcept
        {
            return m_typedData;
        }

        /// @brief gets the underlying data
        /// @return reference of the underlying data
        LLEC_NODISCARD constexpr reference operator*()
        {
            return m_typedData;
        }

        /// @brief gets the underlying data
        /// @return const reference of the underlying data
        LLEC_NODISCARD constexpr const_reference operator*() const
        {
            return m_typedData;
        }

        LLEC_NODISCARD constexpr typed_handle operator+(const typed_handle& other) const
        requires traits::is_addable<raw_type>
        {
            return {m_typedData + other.m_typedData};
        }

        LLEC_NODISCARD constexpr typed_handle operator-(const typed_handle& other) const
        requires traits::is_subtractable<raw_type>
        {
            return {m_typedData - other.m_typedData};
        }

        LLEC_NODISCARD constexpr typed_handle operator*(const typed_handle& other) const
        requires traits::is_multiplyable<raw_type>
        {
            return {m_typedData * other.m_typedData};
        }

        LLEC_NODISCARD constexpr typed_handle operator/(const typed_handle& other) const
        requires traits::is_divideable<raw_type>
        {
            return {m_typedData / other.m_typedData};
        }

        LLEC_NODISCARD constexpr typed_handle operator<<(const typed_handle& other) const
        requires traits::is_left_shiftable<raw_type>
        {
            return {m_typedData << other.m_typedData};
        }

        LLEC_NODISCARD constexpr typed_handle operator>>(const typed_handle& other) const
        requires traits::is_right_shiftable<raw_type>
        {
            return {m_typedData >> other.m_typedData};
        }

        LLEC_NODISCARD constexpr typed_handle operator|(const typed_handle& other) const
        requires traits::is_orable<raw_type>
        {
            return {m_typedData | other.m_typedData};
        }

        LLEC_NODISCARD constexpr typed_handle operator^(const typed_handle& other) const
        requires traits::is_xorable<raw_type>
        {
            return {m_typedData ^ other.m_typedData};
        }

        LLEC_NODISCARD constexpr typed_handle& operator+=(const typed_handle& other)
        requires traits::is_add_assignable<raw_type>
        {
            m_typedData += other.m_typedData;
            return *this;
        }

        LLEC_NODISCARD constexpr typed_handle& operator-=(const typed_handle& other)
        requires traits::is_subtract_assignable<raw_type>
        {
            m_typedData -= other.m_typedData;
            return *this;
        }

        LLEC_NODISCARD constexpr typed_handle& operator*=(const typed_handle& other)
        requires traits::is_multiply_assignable<raw_type>
        {
            m_typedData *= other.m_typedData;
            return *this;
        }

        LLEC_NODISCARD constexpr typed_handle& operator/=(const typed_handle& other)
        requires traits::is_divide_assignable<raw_type>
        {
            m_typedData /= other.m_typedData;
            return *this;
        }

        LLEC_NODISCARD constexpr typed_handle& operator<<=(const typed_handle& other)
        requires traits::is_left_shift_assignable<raw_type>
        {
            m_typedData <<= other.m_typedData;
            return *this;
        }

        LLEC_NODISCARD constexpr typed_handle& operator>>=(const typed_handle& other)
        requires traits::is_right_shift_assignable<raw_type>
        {
            m_typedData >>= other.m_typedData;
            return *this;
        }

        LLEC_NODISCARD constexpr typed_handle& operator|=(const typed_handle& other)
        requires traits::is_or_assignable<raw_type>
        {
            m_typedData |= other.m_typedData;
            return *this;
        }

        LLEC_NODISCARD constexpr typed_handle& operator^=(const typed_handle& other)
        requires traits::is_xor_assignable<raw_type>
        {
            m_typedData ^= other.m_typedData;
            return *this;
        }

        LLEC_NODISCARD constexpr bool operator==(const typed_handle& other) const
        requires traits::is_equal_comparable<raw_type>
        {
            return m_typedData == other.m_typedData;
        }

        LLEC_NODISCARD constexpr bool operator!=(const typed_handle& other) const
        requires traits::is_notequal_comparable<raw_type>
        {
            return m_typedData != other.m_typedData;
        }

        LLEC_NODISCARD constexpr bool operator<(const typed_handle& other) const
        requires traits::is_lessthan_comparable<raw_type>
        {
            return m_typedData < other.m_typedData;
        }

        LLEC_NODISCARD constexpr bool operator<=(const typed_handle& other) const
        requires traits::is_lessthan_equal_comparable<raw_type>
        {
            return m_typedData <= other.m_typedData;
        }

        LLEC_NODISCARD constexpr bool operator>(const typed_handle& other) const
        requires traits::is_greaterthan_comparable<raw_type>
        {
            return m_typedData > other.m_typedData;
        }

        LLEC_NODISCARD constexpr bool operator>=(const typed_handle& other) const
        requires traits::is_greaterthan_equal_comparable<raw_type>
        {
            return m_typedData >= other.m_typedData;
        }

      private:
        raw_type m_typedData;
    };

} // namespace llec
