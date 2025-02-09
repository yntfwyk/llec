/*
 * basic_iterator.hpp
 * provides a basic iterator CRTP interface implementation.
 */

#pragma once
#include <compare>
#include <iterator>

namespace llec
{
    /// @brief An iterator CRTP interface inspired by boost libraries
    /// @tparam Derived Actual iterator type
    /// @tparam Concept Concept or Category
    template <typename Derived, typename Concept, typename Value, typename Reference = Value&,
              typename Pointer = Value*, typename DifferenceType = std::ptrdiff_t>
    class iterator_base
    {
      public:
        using iterator_concept = Concept;
        using iterator_category = Concept;
        using value_type = Value;
        using reference = Reference;
        using pointer = Pointer;
        using difference_type = DifferenceType;

        constexpr reference operator*() const
        {
            return *static_cast<const Derived*>(this)->base_reference();
        }

        constexpr pointer operator->() const
        {
            return static_cast<const Derived*>(this)->base_reference();
        }

        constexpr Derived& operator++()
        {
            auto& it = derived().base_reference();
            ++it;
            return derived();
        }

        constexpr Derived operator++(int)
        {
            auto temp = derived();
            auto& it = derived().base_reference();
            ++it;
            return temp;
        }

        template <typename U = Concept>
        requires std::derived_from<U, std::bidirectional_iterator_tag> 
        constexpr Derived& operator--()
        {
            auto& it = derived().base_reference();
            --it;
            return derived();
        }

        template <typename U = Concept>
        requires std::derived_from<U, std::bidirectional_iterator_tag> 
        constexpr Derived operator--(int)
        {
            auto temp = derived();
            auto& it = derived().base_reference();
            --it;
            return temp;
        }

        template <typename U = Concept>
        requires std::derived_from<U, std::random_access_iterator_tag> 
        constexpr reference operator[](difference_type n) const
        {
            auto it = derived().base_reference();
            return it[n];
        }

        constexpr bool operator==(const Derived& iter) const
        {
            const auto it1 = derived().base_reference();
            const auto it2 = iter.base_reference();
            return it1 == it2;
        }

        constexpr bool operator!=(const Derived& iter) const
        {
            return !(*this == iter);
        }

        constexpr bool operator<(const Derived& d) const
        {
            const auto it1 = derived().base_reference();
            const auto it2 = d.base_reference();
            return it1 < it2;
        }

        constexpr bool operator<=(const Derived& d) const
        {
            const auto it1 = derived().base_reference();
            const auto it2 = d.base_reference();
            return it1 <= it2;
        }

        constexpr bool operator>(const Derived& d) const
        {
            const auto it1 = derived().base_reference();
            const auto it2 = d.base_reference();
            return it1 > it2;
        }

        constexpr bool operator>=(const Derived& d) const
        {
            const auto it1 = derived().base_reference();
            const auto it2 = d.base_reference();
            return it1 >= it2;
        }

        template <typename U = Concept>
        requires std::derived_from<U, std::random_access_iterator_tag> 
        constexpr Derived operator+(difference_type n) const noexcept
        {
            const auto it = derived().base_reference();
            return Derived{it + n};
        }

        template <typename U = Concept>
        requires std::derived_from<U, std::random_access_iterator_tag> 
        constexpr Derived& operator+=(difference_type n) noexcept
        {
            auto& it = derived().base_reference();
            it += n;
            return derived();
        }

        template <typename U = Concept>
        requires std::derived_from<U, std::random_access_iterator_tag> 
        constexpr Derived operator-(difference_type n) const noexcept
        {
            const auto it = derived().base_reference();
            return Derived{it - n};
        }

        template <typename U = Concept>
        requires std::derived_from<U, std::random_access_iterator_tag> 
        constexpr Derived& operator-=(difference_type n) noexcept
        {
            auto& it = derived().base_reference();
            it -= n;
            return derived();
        }

        constexpr difference_type operator-(const Derived& it) const noexcept
        {
            const auto it1 = derived().base_reference();
            const auto it2 = it.base_reference();
            return it1 - it2;
        }

        constexpr difference_type operator+(const Derived& it) const noexcept
        {
            const auto it1 = derived().base_reference();
            const auto it2 = it.base_reference();
            return it1 + it2;
        }

      private:
        constexpr auto& derived()
        {
            return *static_cast<Derived*>(this);
        }

        constexpr const auto& derived() const
        {
            return *static_cast<const Derived*>(this);
        }
    };

    template <typename Derived, typename Concept, typename Value>
    constexpr Derived operator+(const std::iter_difference_t<iterator_base<Derived, Concept, Value>> n,
                                const iterator_base<Derived, Concept, Value> it) noexcept
    {
        return it + n;
    }

} // namespace llec
