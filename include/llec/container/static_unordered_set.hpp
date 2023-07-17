#pragma once
#include "../core/core.hpp"
#include "static_vector.hpp"

namespace llec
{
    /// @brief contiguous set data structure designed for small data sets on the stack
    /// @tparam Key
    /// @tparam Hash hash function
    /// @tparam KeyEqual equal comparator
    /// @tparam Capacity maximum data the structure can hold
    template <typename Key, std::size_t Capacity, typename KeyEqual = std::equal_to<Key>>
    class static_unordered_set
    {
        using storage_type = static_vector<Key, Capacity>;

      public:
        using key_type = Key;
        using value_type = Key;
        using pointer = value_type*;
        using const_pointer = const value_type*;
        using reference = value_type&;
        using const_reference = const value_type&;
        using key_equal = KeyEqual;
        using size_type = std::size_t;
        using iterator = typename storage_type::iterator;
        using const_iterator = typename storage_type::const_iterator;

        constexpr static_unordered_set() noexcept = default;

        constexpr std::pair<iterator, bool> insert(const_reference value) noexcept
        {
            return insert_impl(value);
        }

        constexpr std::pair<iterator, bool> insert(value_type&& value) noexcept
        {
            return insert_impl(std::move(value));
        }

        LLEC_NODISCARD constexpr iterator find(const Key& key) noexcept
        {
            for (auto it = begin(); it != end(); ++it)
            {
                if (m_eq(*it, key))
                {
                    return it;
                }
            }
            return end();
        }

        LLEC_NODISCARD constexpr const_iterator find(const Key& key) const noexcept
        {
            for (auto&& it : std::as_const(m_vector))
            {
                if (m_eq(*it, key))
                {
                    return true;
                }
            }
            return cend();
        }

        LLEC_NODISCARD constexpr bool contains(const Key& key) const noexcept
        {
            return (find(key) != end());
        }

        LLEC_NODISCARD constexpr size_type size() const
        {
            return m_vector.size();
        }

        LLEC_NODISCARD constexpr size_type count(const Key& key) const
        {
            size_type n{};
            for (auto it = cbegin(); it != cend(); ++it)
            {
                if (m_eq(*it, key))
                {
                    n++;
                }
            }
            return n;
        }

        LLEC_NODISCARD constexpr size_type capacity() const noexcept
        {
            return Capacity;
        }

        LLEC_NODISCARD constexpr iterator begin() noexcept
        {
            return m_vector.begin();
        }

        LLEC_NODISCARD constexpr iterator end() noexcept
        {
            return m_vector.end();
        }

        LLEC_NODISCARD constexpr const_iterator begin() const noexcept
        {
            return m_vector.cbegin();
        }

        LLEC_NODISCARD constexpr const_iterator end() const noexcept
        {
            return m_vector.cend();
        }

        LLEC_NODISCARD constexpr const_iterator cbegin() const noexcept
        {
            return m_vector.cbegin();
        }

        LLEC_NODISCARD constexpr const_iterator cend() const noexcept
        {
            return m_vector.cend();
        }

      private:
        template <typename T>
        LLEC_NODISCARD constexpr std::pair<iterator, bool> insert_impl(T&& value) noexcept
        {
            iterator it = find(std::forward<T>(value));
            static_assert(std::is_rvalue_reference_v<decltype(std::forward<T>(value))>);
            if (it != end())
            {
                return {it, false};
            }
            return {m_vector.insert(it, std::forward<T>(value)), true};
        }

      private:
        storage_type m_vector{};
        key_equal m_eq;
    };
} // namespace llec