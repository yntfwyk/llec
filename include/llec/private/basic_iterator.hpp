/*
 * basic_iterator.hpp
 * provides a basic contiguous iterator implementation.
 */

#pragma once
#include <iterator>

namespace llec
{
    namespace details
    {
        template <typename T>
        class basic_contiguous_iterator
        {
          public:
            // using iterator_concept = typename std::contiguous_iterator_tag;
            using iterator_category = typename std::random_access_iterator_tag;
            using difference_type = std::ptrdiff_t;
            using value_type = T;
            using pointer = T*;
            using reference = T&;

            basic_contiguous_iterator() = default;
            constexpr basic_contiguous_iterator(T* data) noexcept : ptr(data)
            {
            }

            virtual constexpr reference operator*() const noexcept
            {
                return *ptr;
            }

            virtual constexpr pointer operator->() const noexcept
            {
                return ptr;
            }

            virtual constexpr basic_contiguous_iterator& operator++() noexcept
            {
                ++ptr;
                return *this;
            }

            virtual constexpr basic_contiguous_iterator operator++(int) noexcept
            {
                const auto tmp(*this);
                ++(*this);
                return tmp;
            }

            virtual constexpr basic_contiguous_iterator operator+(difference_type n) const noexcept
            {
                return basic_contiguous_iterator{ptr + n};
            }

            virtual constexpr basic_contiguous_iterator& operator+=(difference_type n) noexcept
            {
                ptr += n;
                return *this;
            }

            virtual constexpr basic_contiguous_iterator& operator--() noexcept
            {
                --ptr;
                return *this;
            }

            virtual constexpr basic_contiguous_iterator operator--(int) noexcept
            {
                const auto tmp(*this);
                --(*this);
                return tmp;
            }

            virtual constexpr basic_contiguous_iterator operator-(difference_type n) const noexcept
            {
                return basic_contiguous_iterator{ptr - n};
            }

            virtual constexpr basic_contiguous_iterator& operator-=(difference_type n) noexcept
            {
                ptr -= n;
                return *this;
            }

            virtual constexpr difference_type operator-(basic_contiguous_iterator it) const noexcept
            {
                return ptr - it.ptr;
            }

            virtual constexpr reference operator[](difference_type n) const
            {
                return ptr[n];
            }

            virtual constexpr bool operator!=(const basic_contiguous_iterator& other) const noexcept
            {
                return ptr != other.ptr;
            }

            virtual constexpr bool operator==(const basic_contiguous_iterator& other) const noexcept
            {
                return ptr == other.ptr;
            }

            virtual constexpr bool operator<(const basic_contiguous_iterator& other) const noexcept
            {
                return ptr < other.ptr;
            }

            virtual constexpr bool operator<=(const basic_contiguous_iterator& other) const noexcept
            {
                return ptr <= other.ptr;
            }

            virtual constexpr bool operator>(const basic_contiguous_iterator& other) const noexcept
            {
                return ptr > other.ptr;
            }

            virtual constexpr bool operator>=(const basic_contiguous_iterator& other) const noexcept
            {
                return ptr >= other.ptr;
            }

          private:
            T* ptr{nullptr};
        };

        template <typename T>
        using basic_contiguous_const_iterator = basic_contiguous_iterator<const T>;

        template <typename T>
        constexpr basic_contiguous_iterator<T> operator+(const std::iter_difference_t<basic_contiguous_iterator<T>> n,
                                                         const basic_contiguous_iterator<T> it) noexcept
        {
            return it + n;
        }

        template <typename T>
        constexpr basic_contiguous_const_iterator<T> operator+(
            const std::iter_difference_t<basic_contiguous_const_iterator<T>> n,
            const basic_contiguous_const_iterator<T> it) noexcept
        {
            return it + n;
        }
    } // namespace details
} // namespace llec
