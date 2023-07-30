/*
 * small_unordered_set.hpp
 * A contiguous unordered set data structure for small data sets on the stack.
 * Insertions, deletions and search are O(n) complexity
 * Provides good performance when iterating as the data is contiguous in memory.
 */

#pragma once
#include "fixed_vector.hpp"

namespace llec
{
    /// @brief contiguous set data structure designed for small data sets on the stack
    /// @tparam Key
    /// @tparam Capacity maximum data the structure can hold
    /// @tparam KeyEqual equal comparator
    template <typename Key, std::size_t Capacity, typename KeyEqual = std::equal_to<Key>>
    class small_unordered_set
    {
        using storage_type = fixed_vector<Key, Capacity>;

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
        using difference_type = typename storage_type::difference_type;

        /// @brief inserts a key
        /// @param key
        /// @return std:pair of the position if the element was successfully inserted
        /// and a bool which is set to true on a successful insertion, else set to false
        constexpr std::pair<iterator, bool> insert(const Key& key) noexcept
        {
            return insert_impl(key);
        }

        /// @brief inserts a key
        /// @param key
        /// @return std:pair of the position if the element was successfully inserted
        /// and a bool which is set to true on a successful insertion, else set to false
        constexpr std::pair<iterator, bool> insert(Key&& key) noexcept
        {
            return insert_impl(std::move(key));
        }

        /// @brief inserts a range of keys
        /// @tparam InputIt iterator type
        /// @param ibegin
        /// @param iend
        template <typename InputIt>
        constexpr void insert(InputIt ibegin, InputIt iend) noexcept
        {
            for (auto it = ibegin; it != iend; ++it)
            {
                insert(*it);
            }
        }

        /// @brief range inserts keys from an initializer list
        /// @param ilist list that holds the keys
        constexpr void insert(std::initializer_list<value_type> ilist) noexcept
        {
            for (auto it = ilist.begin(); it != ilist.end(); ++it)
            {
                insert(*it);
            }
        }

        /// @brief removes a key at a position
        /// @param it position
        /// @return iterator following the last removed element
        constexpr iterator erase(iterator it) noexcept
        {
            return m_vector.erase(it);
        }

        /// @brief removes a key at a position
        /// @param it position
        /// @return iterator following the last removed element
        constexpr iterator erase(const_iterator it) noexcept
        {
            return m_vector.erase(it);
        }

        /// @brief removes keys within a range
        /// @tparam It iteratir type (can be non-const)
        /// @param first starting position
        /// @param last ending position
        /// @return iterator following the last removed element
        template <typename It>
            requires(std::is_same_v<It, iterator> || std::is_same_v<It, const_iterator>)
        constexpr iterator erase(It first, It last) noexcept
        {
            return m_vector.erase(first, last);
        }

        /// @brief removes a key matching the input
        /// @param key key to search and remove
        /// @return true on successful removal, else false
        constexpr bool erase(const Key& key) noexcept
        {
            if (auto it = find(key); it != end())
            {
                erase(it);
                return true;
            }
            return false;
        }

        /// @brief removes all the keys
        constexpr void clear() noexcept
        {
            m_vector.clear();
        }

        /// @brief searches for a specific key
        /// @param key key to find
        /// @return position of the key if found else returns the end iterator
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

        /// @brief searches for a specific key
        /// @param key key to find
        /// @return position of the key if found else returns the end iterator
        LLEC_NODISCARD constexpr const_iterator find(const Key& key) const noexcept
        {
            for (auto it = cbegin(); it != cend(); ++it)
            {
                if (m_eq(*it, key))
                {
                    return it;
                }
            }
            return cend();
        }

        /// @brief checks if a key is present
        /// @param key key to look for
        /// @return true if found, else false
        LLEC_NODISCARD constexpr bool contains(const Key& key) const noexcept
        {
            return (find(key) != end());
        }

        /// @brief size of the set
        /// @return number of keys
        LLEC_NODISCARD constexpr size_type size() const
        {
            return m_vector.size();
        }

        [[deprecated("Prefer using contains instead")]] LLEC_NODISCARD constexpr size_type count(const Key& key) const
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

        /// @brief capacity
        /// @return maximum number of keys the structure can hold
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
            if (it != end())
            {
                return {it, false};
            }
            return {m_vector.insert(it, std::forward<T>(value)), true};
        }

      private:
        storage_type m_vector;
        key_equal m_eq;
    };
} // namespace llec