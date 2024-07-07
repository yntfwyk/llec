/*
 * fixed_string.hpp
 * A basic string class focusing on stack and compile-time operations.
 */

#pragma once
#include "core/core.hpp"
#include "private/basic_iterator.hpp"

namespace llec
{
    /// @brief A basic null terminated string class focusing on stack and compile-time operations.
    /// @tparam Capacity constant size
    template <std::size_t Capacity = 512ull>
    class fixed_string
    {
      public:
        using iterator = details::basic_contiguous_iterator<char>;
        using const_iterator = details::basic_contiguous_const_iterator<char>;

        constexpr fixed_string() noexcept = default;

        /// @brief parameterized constructor
        /// @param string takes a const char* string
        explicit constexpr fixed_string(const char* string) noexcept
        {
            LLEC_ASSERT(string);
            const std::size_t len = fixed_string_helper::const_string_length(string);
            if (!len)
                clear();
            else
            {
                const std::size_t charsToCopy = len > Capacity - 1 ? Capacity - 1 : len;
                fixed_string_helper::copy_n(m_string, charsToCopy, string);
                m_string[charsToCopy] = 0;
            }
        }

        constexpr fixed_string& operator=(const char* string) noexcept
        {
            LLEC_ASSERT(string);
            const std::size_t len = fixed_string_helper::const_string_length(string);
            if (!len)
                clear();
            else
            {
                const std::size_t charsToCopy = len > Capacity - 1 ? Capacity - 1 : len;
                fixed_string_helper::copy_n(m_string, charsToCopy, string);
                m_string[charsToCopy] = 0;
            }
            return *this;
        }

        constexpr char operator[](std::size_t n) const noexcept
        {
            return m_string[n];
        }

        constexpr char& operator[](std::size_t n) noexcept
        {
            return m_string[n];
        }

        constexpr fixed_string operator+(const fixed_string& other) const noexcept
        {
            if (other.is_empty())
                return {};

            const std::size_t len = length();
            const std::size_t availability = len < Capacity - 1u ? Capacity - 1u - len : 0u;
            if (!availability)
                return *this;
            fixed_string result;
            fixed_string_helper::copy_n(result.m_string, len, m_string);
            fixed_string_helper::copy_n(result.m_string + len, availability, other.m_string);
            return result;
        }

        constexpr bool operator==(const fixed_string& other) const noexcept
        {
            const std::size_t len = length();
            if (len != other.length())
                return false;
            if (std::is_constant_evaluated())
            {
                for (std::size_t i = 0; i < len; i++)
                    if (m_string[i] != other.m_string[i])
                        return false;
                return true;
            }
            else
            {
                return !std::memcmp(m_string, other.m_string, len);
            }
        }

        constexpr bool operator!=(const fixed_string& other) const noexcept
        {
            return !operator==(other);
        }

        LLEC_NODISCARD constexpr const char* data() const noexcept
        {
            return m_string;
        }

        /// @brief returns length of the fixed_string
        /// @return length of type size_t
        LLEC_NODISCARD constexpr std::size_t length() const noexcept
        {
            return fixed_string_helper::const_string_length(m_string);
        }

        /// @brief returns constant capacity of fixed_string
        /// @return capacity of type size_t
        LLEC_NODISCARD static constexpr std::size_t capacity()
        {
            return Capacity;
        }

        /// @brief clears the fixed_string
        constexpr void clear() noexcept
        {
            m_string[0] = 0;
        }

        /// @brief checks if fixed_string is empty
        /// @return true if empty, else returns false
        LLEC_NODISCARD constexpr bool is_empty() const noexcept
        {
            return !static_cast<bool>(m_string[0]);
        }

        /// @brief returns substring
        /// @param pos start position
        /// @param n number of characters
        /// @return substring of type fixed_string
        LLEC_NODISCARD constexpr fixed_string substr(std::size_t pos, std::size_t n) const noexcept
        {
            LLEC_ASSERT(!is_empty());
            const std::size_t srcLen = length();
            LLEC_ASSERT(!(pos >= srcLen || !n));

            fixed_string result;
            const std::size_t availability = srcLen - pos;
            n = n > availability ? availability : n;
            fixed_string_helper::copy_n(result.m_string, n, m_string + pos);
            result[n] = 0;
            return result;
        }

        /// @brief finds a pattern in the fixed_string
        /// @param pattern sequence of characters of type fixed_string
        /// @return if found returns starting position of the pattern in the string
        /// else, returns -1
        LLEC_NODISCARD constexpr s32 find(const fixed_string& pattern) const noexcept
        {
            if (!pattern.is_empty())
            {
                const std::size_t patternLength = pattern.length();
                const std::size_t len = length();
                for (std::size_t i = 0; i <= len - patternLength; i++)
                {
                    std::size_t j = 0;
                    while (j < patternLength && m_string[i + j] == pattern[j])
                        j++;
                    if (j == patternLength)
                        return static_cast<s32>(i);
                }
            }
            return -1;
        }

        /// @brief appends string to the end
        /// @param str string to append
        /// @param n number of characters to append (must be <= length())
        /// @return reference to the result
        constexpr fixed_string& append(const char* str, std::size_t n) noexcept
        {
            LLEC_ASSERT(str);
            const auto size = length();
            const std::size_t availability = Capacity - size - 1;
            const auto sizeStr = n;
            const auto copySize = availability >= sizeStr ? sizeStr : availability;
            fixed_string_helper::copy_n(std::addressof(m_string[size]), copySize, str);
            m_string[size + copySize] = 0;
            return *this;
        }

        /// @brief appends string to the end
        /// @param str string to append
        /// @return reference to the result
        constexpr fixed_string& append(const char* str) noexcept
        {
            return append(str, fixed_string_helper::const_string_length(str));
        }

        /// @brief appends string to the end
        /// @param str string to append
        /// @return reference to the result
        constexpr fixed_string& append(const fixed_string& str) noexcept
        {
            return append(str.data(), fixed_string_helper::const_string_length(str.data()));
        }

        LLEC_NODISCARD constexpr const_iterator cbegin() const noexcept
        {
            return const_iterator{m_string};
        }

        LLEC_NODISCARD constexpr const_iterator cend() const noexcept
        {
            return const_iterator{std::addressof(m_string[length()])};
        }

        LLEC_NODISCARD constexpr iterator begin() noexcept
        {
            return iterator{m_string};
        }

        LLEC_NODISCARD constexpr iterator end() noexcept
        {
            return iterator{std::addressof(m_string[length()])};
        }

        LLEC_NODISCARD constexpr const_iterator begin() const noexcept
        {
            return const_iterator{m_string};
        }

        LLEC_NODISCARD constexpr const_iterator end() const noexcept
        {
            return const_iterator{std::addressof(m_string[length()])};
        }

      private:
        struct fixed_string_helper
        {
            static constexpr std::size_t const_string_length(const char* string) noexcept
            {
                std::size_t i{0};
                while (string[i++])
                    ;
                return i - 1;
            }

            static constexpr void fill_n(char* string, std::size_t n, char value) noexcept
            {
                for (std::size_t i = 0; i < n; i++)
                    string[i] = value;
            }

            static constexpr void copy_n(char* dest, std::size_t n, const char* src) noexcept
            {
                for (std::size_t i = 0; i < n; i++)
                    dest[i] = src[i];
            }
        };

      private:
        char m_string[Capacity]{};
    };

    /// @brief fixed_string of capacity 64bytes
    using fixed_string64 = fixed_string<64>;

    /// @brief fixed_string of capacity 128bytes
    using fixed_string128 = fixed_string<128>;

    /// @brief fixed_string of capacity 256bytes
    using fixed_string256 = fixed_string<256>;

    /// @brief fixed_string of capacity 1024bytes
    using fixed_string1024 = fixed_string<1024>;
} // namespace llec
