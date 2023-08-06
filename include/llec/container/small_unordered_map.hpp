/*
 * small_unordered_map.hpp
 * A contiguous unordered map data structure for small data sets on the stack.
 * Insertions, deletions and search are O(n) complexity
 * Provides good performance when iterating as the data is contiguous in memory.
 */

#pragma once
#include "fixed_vector.hpp"
#include "utility/pair.hpp"

namespace llec
{
    template <typename Key, typename Value, std::size_t Capacity, typename KeyEqual = std::equal_to<Key>>
    class small_unordered_map
    {
        template <typename Type>
        using storage_type = fixed_vector<Type, Capacity>;

        template <typename K, typename V, bool IsConst = false>
        class small_umap_iterator;

      public:
        using key_type = Key;
        using mapped_type = Value;
        /// @brief as this structure is based of fixed_vector and vectors does not allow cv qualified types
        /// we have to use non-const version of Key in the pair
        using value_type = pair<Key, Value>;
        using pointer = value_type*;
        using const_pointer = const value_type*;
        using reference = value_type&;
        using const_reference = const value_type&;
        using key_equal = KeyEqual;
        using size_type = std::size_t;
        using iterator = small_umap_iterator<Key, Value>;
        using const_iterator = small_umap_iterator<Key, Value, true>;
        using difference_type = typename storage_type<mapped_type>::difference_type;

        /// @brief inserts a value
        /// @param value
        /// @return std:pair of the position if the element was successfully inserted
        /// and a bool which is set to true on a successful insertion, else set to false
        constexpr pair<iterator, bool> insert(const_reference value) noexcept
        {
            return insert_impl(value);
        }

        /// @brief inserts a value
        /// @param value
        /// @return std:pair of the position if the element was successfully inserted
        /// and a bool which is set to true on a successful insertion, else set to false
        constexpr pair<iterator, bool> insert(value_type&& value) noexcept
        {
            return insert_impl(std::move(value));
        }

        /// @brief inserts a range of values
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

        /// @brief range inserts values from an initializer list
        /// @param ilist list that holds the keys
        constexpr void insert(std::initializer_list<value_type> ilist) noexcept
        {
            for (auto it = ilist.begin(); it != ilist.end(); ++it)
            {
                insert(*it);
            }
        }

        /// @brief removes a value at a position
        /// @param it position
        /// @return iterator following the last removed element
        constexpr iterator erase(iterator it) noexcept
        {
            return iterator{typename iterator::key_iterator{std::addressof(*(m_keys.erase(it.get_key_iterator())))}, m_values.erase(it.get_value_iterator())};
        }

        /// @brief removes a value at a position
        /// @param it position
        /// @return iterator following the last removed element
        constexpr iterator erase(const_iterator it) noexcept
        {
            m_keys.erase(m_keys.begin() + std::distance(m_values.cbegin(), it));
            return m_values.erase(it);
        }

        /// @brief removes values within a range
        /// @tparam It iteratir type (can be non-const)
        /// @param first starting position
        /// @param last ending position
        /// @return iterator following the last removed element
        template <typename It>
            requires(std::is_same_v<It, iterator> || std::is_same_v<It, const_iterator>)
        constexpr iterator erase(It first, It last) noexcept
        {
            return {typename iterator::key_iterator{std::addressof(*(m_keys.erase(first.get_key_iterator(), last.get_key_iterator())))}, m_values.erase(first.get_value_iterator(), last.get_value_iterator())};
        }

        /// @brief removes a value matching the key
        /// @param key key to search
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
            m_keys.clear();
            m_values.clear();
        }

        /// @brief searches for a specific key
        /// @param key key to find
        /// @return position of the key if found else returns the end iterator
        LLEC_NODISCARD constexpr iterator find(const key_type& key) noexcept
        {
            size_type i = 0;
            for (auto it = m_keys.begin(); it != m_keys.end(); ++it, ++i)
            {
                if (m_eq(*it, key))
                {
                    return (begin() + i);
                }
            }
            return end();
        }

        /// @brief searches for a specific key
        /// @param key key to find
        /// @return position of the key if found else returns the end iterator
        LLEC_NODISCARD constexpr const_iterator find(const key_type& key) const noexcept
        {
            auto itEnd = cend();
            for (auto&& it = cbegin(); it != itEnd; ++it)
            {
                if (m_eq(it.get_key(), key))
                {
                    return it;
                }
            }
            return itEnd;
        }

        /// @brief checks if a key is present
        /// @param key key to look for
        /// @return true if found, else false
        LLEC_NODISCARD constexpr bool contains(const key_type& key) const noexcept
        {
            return (find(key) != end());
        }

        /// @brief value at specified key
        /// Note: if the key is not present no exception is thrown and will lead to undefined behavior
        /// @param key
        /// @return reference to the value
        LLEC_NODISCARD constexpr mapped_type& at(const key_type& key) noexcept
        {
            return find(key).get_value();
        }

        /// @brief value at specified key
        /// Note: if the key is not present no exception is thrown and will lead to undefined behavior
        /// @param key
        /// @return const reference to the value
        LLEC_NODISCARD constexpr const mapped_type& at(const key_type& key) const noexcept
        {
            return find(key).get_value();
        }

        /// @brief size of the map
        /// @return number of keys
        LLEC_NODISCARD constexpr size_type size() const
        {
            LLEC_ASSERT(m_values.size() == m_keys.size());
            return m_values.size();
        }

        /// @brief capacity
        /// @return maximum number of keys the structure can hold
        LLEC_NODISCARD constexpr size_type capacity() const noexcept
        {
            return Capacity;
        }

        LLEC_NODISCARD constexpr iterator begin() noexcept
        {
            return iterator{m_keys.cbegin(), m_values.begin()};
        }

        LLEC_NODISCARD constexpr iterator end() noexcept
        {
            return iterator{m_keys.cend(), m_values.end()};
        }

        LLEC_NODISCARD constexpr const_iterator begin() const noexcept
        {
            return const_iterator{m_keys.cbegin(), m_values.cbegin()};
        }

        LLEC_NODISCARD constexpr const_iterator end() const noexcept
        {
            return const_iterator{m_keys.cend(), m_values.cend()};
        }

        LLEC_NODISCARD constexpr const_iterator cbegin() const noexcept
        {
            return const_iterator{m_keys.cbegin(), m_values.cbegin()};
        }

        LLEC_NODISCARD constexpr const_iterator cend() const noexcept
        {
            return const_iterator{m_keys.cend(), m_values.cend()};
        }

        LLEC_NODISCARD constexpr mapped_type& operator[](const key_type& key)
        {
            if (auto it = find(key); it != end())
                LLEC_LIKELY
                {
                    return (*it).second;
                }
            else
            {
                m_keys.push_back(key);
                m_values.push_back(mapped_type{});
                return m_values.back();
            }
        }

        LLEC_NODISCARD constexpr mapped_type& operator[](key_type&& key)
        {
            if (auto it = find(key); it != end())
                LLEC_LIKELY
                {
                    return it.get_value();
                }
            else
            {
                m_keys.push_back(std::move(key));
                m_values.push_back(mapped_type{});
                return m_values.back();
            }
        }

      private:
        template <typename K, typename V, bool IsConst>
        class small_umap_iterator
        {
            using key_iterator = typename storage_type<K>::const_iterator;
            using value_iterator = std::conditional_t<IsConst, typename storage_type<V>::const_iterator, typename storage_type<V>::iterator>;

          public:
            using iterator_concept = typename std::contiguous_iterator_tag;
            using iterator_category = typename std::random_access_iterator_tag;
            using difference_type = typename key_iterator::difference_type;
            using value_type = pair<key_iterator, value_iterator>;
            using pointer = std::conditional_t<IsConst, const value_type*, value_type*>;
            using reference = std::conditional_t<IsConst, const value_type&, value_type&>;

            small_umap_iterator() = default;
            constexpr small_umap_iterator(key_iterator kIt, value_iterator vIt) noexcept : m_keyIter(kIt), m_valueIter(vIt)
            {
            }

            value_type operator*() const
            {
                return {m_keyIter, m_valueIter};
            }

            constexpr const K& get_key() const noexcept
            {
                return *m_keyIter;
            }

            constexpr const V& get_value() const noexcept
            {
                return *m_valueIter;
            }

            constexpr V& get_value() noexcept
            {
                return const_cast<V&>(*m_valueIter);
            }

            constexpr small_umap_iterator& operator++() noexcept
            {
                ++m_keyIter;
                ++m_valueIter;
                return *this;
            }

            constexpr small_umap_iterator operator++(int) noexcept
            {
                const auto tmp(*this);
                ++(*this);
                return tmp;
            }

            constexpr small_umap_iterator& operator--() noexcept
            {
                --m_keyIter;
                --m_valueIter;
                return *this;
            }

            constexpr small_umap_iterator operator--(int) noexcept
            {
                const auto tmp(*this);
                --(*this);
                return tmp;
            }

            constexpr small_umap_iterator operator+(difference_type diff) const noexcept
            {
                return {m_keyIter + diff, m_valueIter + diff};
            }

            constexpr small_umap_iterator operator-(difference_type diff) const noexcept
            {
                return {m_keyIter - diff, m_valueIter - diff};
            }

            constexpr bool operator==(const small_umap_iterator& other) const noexcept
            {
                return m_keyIter == other.m_keyIter && m_valueIter == other.m_valueIter;
            }

            constexpr bool operator!=(const small_umap_iterator& other) const noexcept
            {
                return !operator==(other);
            }

          private:
            constexpr key_iterator get_key_iterator() const noexcept
            {
                return m_keyIter;
            }

            constexpr value_iterator get_value_iterator() const noexcept
            {
                return m_valueIter;
            }

            key_iterator m_keyIter{nullptr};
            value_iterator m_valueIter{nullptr};

            friend small_unordered_map<Key, Value, Capacity, KeyEqual>;
        };

      private:
        template <typename T>
        LLEC_NODISCARD constexpr pair<iterator, bool> insert_impl(T&& value) noexcept
        {
            iterator it = find(std::forward<T>(value).first);
            if (it != end())
            {
                return {it, false};
            }
            m_keys.push_back(std::forward<T>(value).first);
            return {iterator{m_keys.cend() - 1, m_values.insert(it.get_value_iterator(), std::forward<T>(value).second)}, true};
        }

      private:
        storage_type<key_type> m_keys;
        storage_type<mapped_type> m_values;
        key_equal m_eq;
    };
} // namespace llec