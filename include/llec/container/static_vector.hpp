/*
 * static_vector.hpp
 * A vector on the stack, almost every function is marked constexpr
 * so we can expect compile time evaluations.
 * Not all functionality is present from std::vector, though the crucial and important ones are implemented.
 */

#pragma once
#include "../core/core.hpp"
#include "../private/aligned_storage.hpp"
#include "../private/basic_iterator.hpp"
#include <algorithm>
#include <array>

namespace llec
{
    /// @brief A vector on the stack
    /// @tparam T Type
    /// @tparam Capacity Maximum number of elements the vector can hold
    template <typename T, std::size_t Capacity>
    class static_vector
    {
        using storage_type = std::conditional_t<traits::is_trivially_xstructible_v<T>, std::array<T, Capacity>,
                                                details::aligned_storage_t<sizeof(T) * Capacity, alignof(T)>>;

      public:
        using iterator = details::basic_contiguous_iterator<T>;
        using const_iterator = details::basic_contiguous_const_iterator<T>;
        using value_type = T;
        using pointer = T*;
        using const_pointer = const T*;
        using reference = T&;
        using const_reference = const T&;
        using size_type = std::size_t;
        using difference_type = std::ptrdiff_t;

        constexpr static_vector() noexcept = default;
        constexpr ~static_vector() noexcept = default;
        constexpr static_vector(std::initializer_list<value_type> il) noexcept
        {
            insert(cbegin(), il.begin(), il.end());
        }
        constexpr static_vector(const static_vector&) noexcept = default;
        constexpr static_vector& operator=(const static_vector&) noexcept = default;
        constexpr static_vector(static_vector&& other) noexcept
        {
            move_all(std::move(other));
        }
        constexpr static_vector& operator=(static_vector&& other) noexcept
        {
            move_all(std::move(other));
            return *this;
        }

        /// @brief pushes value to the back of the vector
        /// @param value
        constexpr void push_back(const value_type& value) noexcept
        {
            LLEC_ASSERT(m_count < Capacity);
            std::construct_at(get_data_address(m_count), value);
            m_count++;
        }

        /// @brief pushes value to the back of the vector
        /// @param value
        constexpr void push_back(value_type&& value) noexcept
        {
            LLEC_ASSERT(m_count < Capacity);
            std::construct_at(get_data_address(m_count), std::move(value));
            m_count++;
        }

        /// @brief constructs the data in-place
        /// @tparam ...Args
        /// @param ...args
        template <typename... Args>
        constexpr void emplace_back(Args&&... args) noexcept
        {
            LLEC_ASSERT(m_count < Capacity);
            std::construct_at(get_data_address(m_count), std::forward<Args>(args)...);
            m_count++;
        }

        /// @brief constructs the data at the specified position in the vector
        /// @tparam
        /// @tparam ConstIt
        /// @tparam ...Args
        /// @param pos
        /// @param ...args
        /// @return
        template <
            typename ConstIt,
            typename = std::enable_if_t<std::is_same_v<ConstIt, iterator> || std::is_same_v<ConstIt, const_iterator>>,
            typename... Args>
        iterator emplace(ConstIt pos, Args&&... args) noexcept
        {
            LLEC_ASSERT(m_count < Capacity);

            const_iterator _pos{std::addressof(*pos)};
            LLEC_ASSERT(_pos <= cend());

            if (m_count == 0)
            {
                std::construct_at(get_data_address(), std::forward<Args>(args)...);
            }
            else if (_pos == cend())
            {
                std::construct_at(get_data_address(m_count), std::forward<Args>(args)...);
            }
            else
            {
                std::memcpy(const_cast<typename iterator::pointer>(std::addressof(*(_pos + 1))), std::addressof(*_pos),
                            (cend() - _pos) * sizeof(value_type));
                std::construct_at(std::addressof(*_pos), std::forward<Args>(args)...);
            }
            m_count++;

            if constexpr (std::is_same_v<ConstIt, const_iterator>)
            {
                return iterator{const_cast<typename iterator::pointer>(std::addressof(*_pos))};
            }
            else
            {
                return pos;
            }
        }

        /// @brief inserts data at the specified position in the vector
        /// @tparam ConstIt
        /// @tparam
        /// @param pos
        /// @param value
        /// @return
        template <typename ConstIt, typename = std::enable_if_t<std::is_same_v<ConstIt, iterator> ||
                                                                std::is_same_v<ConstIt, const_iterator>>>
        constexpr iterator insert(ConstIt pos, const value_type& value) noexcept
        {
            return emplace(pos, value);
        }

        /// @brief inserts data at the specified position in the vector
        /// @tparam ConstIt
        /// @tparam
        /// @param pos
        /// @param value
        /// @return
        template <typename ConstIt, typename = std::enable_if_t<std::is_same_v<ConstIt, iterator> ||
                                                                std::is_same_v<ConstIt, const_iterator>>>
        constexpr iterator insert(ConstIt pos, value_type&& value) noexcept
        {
            return emplace(pos, std::move(value));
        }

        /// @brief inserts range of data at the specified position in the vector
        /// @tparam ConstIt
        /// @tparam
        /// @tparam InputIt
        /// @param pos
        /// @param ibegin
        /// @param iend
        /// @return
        template <
            typename ConstIt, typename InputIt,
            typename = std::enable_if_t<std::is_same_v<ConstIt, iterator> || std::is_same_v<ConstIt, const_iterator>>>
        constexpr iterator insert(ConstIt pos, InputIt ibegin, InputIt iend) noexcept
        {
            LLEC_ASSERT(m_count < Capacity);

            const size_type count = std::distance(ibegin, iend);
            LLEC_ASSERT(count <= (Capacity - m_count));

            LLEC_ASSERT(ibegin < iend);

            const_iterator _pos{std::addressof(*pos)};
            LLEC_ASSERT(_pos <= cend());

            if (m_count == 0)
            {
                for (size_type i = 0; i < count; i++)
                {
                    std::construct_at(get_data_address(i), *(ibegin + i));
                }
            }
            else if (_pos == cend())
            {
                for (size_type i = 0; i < count; i++)
                {
                    std::construct_at(get_data_address(m_count + i), *(ibegin + i));
                }
            }
            else
            {
                std::memcpy(const_cast<typename iterator::pointer>(std::addressof(*(_pos + count))),
                            std::addressof(*_pos), (cend() - _pos) * sizeof(value_type));
                for (size_type i = 0; i < count; i++)
                {
                    std::construct_at(std::addressof(*(_pos + i)), *(ibegin + i));
                }
            }

            m_count += count;

            if constexpr (std::is_same_v<ConstIt, const_iterator>)
            {
                return iterator{const_cast<typename iterator::pointer>(std::addressof(*_pos))};
            }
            else
            {
                return pos;
            }
        }

        /// @brief pops off data from the back of the vector
        constexpr void pop_back() noexcept
        {
            LLEC_ASSERT(m_count > 0 && m_count <= Capacity);
            std::destroy_at(get_data_address(--m_count));
        }

        /// @brief removes data from the specified position in the vector
        /// @tparam It
        /// @tparam
        /// @param pos
        /// @return
        template <typename It,
                  typename = std::enable_if_t<std::is_same_v<It, iterator> || std::is_same_v<It, const_iterator>>>
        constexpr iterator erase(It pos) noexcept
        {
            LLEC_ASSERT(m_count > 0);
            It itEnd = iterator_selector<It, false>();
            LLEC_ASSERT(pos < itEnd);
            std::destroy_at(std::addressof(*pos));
            m_count--;
            if (pos < (itEnd - 1))
            {
                std::memcpy(std::addressof(*iterator_adapter(pos)), std::addressof(*(pos + 1)),
                            (itEnd - (pos + 1)) * sizeof(value_type));
                return iterator_adapter(pos);
            }
            return end();
        }

        /// @brief removes range of data
        /// @tparam It
        /// @tparam
        /// @param ibegin
        /// @param iend
        /// @return
        template <typename It,
                  typename = std::enable_if_t<std::is_same_v<It, iterator> || std::is_same_v<It, const_iterator>>>
        constexpr iterator erase(It ibegin, It iend) noexcept
        {
            LLEC_ASSERT(m_count > 0);
            LLEC_ASSERT(ibegin < iend);
            if (ibegin == iend) LLEC_UNLIKELY
            {
                return iterator_adapter(iend);
            }

            const size_type count = std::distance(ibegin, iend);
            LLEC_ASSERT(count <= m_count);

            It itEnd = iterator_selector<It, false>();

            m_count -= count;
            std::destroy_n(ibegin, count);
            if (iend == itEnd)
            {
                return end();
            }

            auto first = iterator_adapter(ibegin);
            std::memcpy(std::addressof(*first), std::addressof(*iend), (itEnd - iend) * sizeof(value_type));
            return first;
        }

        /// @brief clears the vector i.e. all elements will destroyed and size will be set to 0
        void clear() noexcept
        {
            erase(begin(), end());
        }

        /// @brief return the number of elements in the vector
        /// @return
        LLEC_NODISCARD constexpr size_type size() const noexcept
        {
            return m_count;
        }

        /// @brief returns max capacity of the vector
        /// @return
        LLEC_NODISCARD constexpr size_type capacity() const noexcept
        {
            return Capacity;
        }

        LLEC_NODISCARD constexpr const_iterator cbegin() const noexcept
        {
            return const_iterator{get_data_address()};
        }

        LLEC_NODISCARD constexpr const_iterator cend() const noexcept
        {
            return const_iterator{get_data_address(m_count)};
        }

        LLEC_NODISCARD constexpr iterator begin() noexcept
        {
            return iterator{get_data_address()};
        }

        LLEC_NODISCARD constexpr iterator end() noexcept
        {
            return iterator{get_data_address(m_count)};
        }

        LLEC_NODISCARD constexpr const_iterator begin() const noexcept
        {
            return const_iterator{get_data_address()};
        }

        LLEC_NODISCARD constexpr const_iterator end() const noexcept
        {
            return const_iterator{get_data_address(m_count)};
        }

        LLEC_NODISCARD constexpr reference operator[](size_type index) noexcept
        {
            LLEC_ASSERT(index < Capacity);
            return *get_data_address(index);
        }

        LLEC_NODISCARD constexpr const_reference operator[](size_type index) const noexcept
        {
            LLEC_ASSERT(index < Capacity);
            return *get_data_address(index);
        }

      private:
        LLEC_NODISCARD constexpr T* get_data_address(size_type index = 0) noexcept
        {
            if constexpr (traits::is_trivially_xstructible_v<value_type>)
            {
                return (m_data.data() + index);
            }
            else
            {
                return reinterpret_cast<T*>(std::addressof(m_data)) + index;
            }
        }

        LLEC_NODISCARD constexpr const T* get_data_address(size_type index = 0) const noexcept
        {
            if constexpr (traits::is_trivially_xstructible_v<value_type>)
            {
                return (m_data.data() + index);
            }
            else
            {
                return reinterpret_cast<const T*>(std::addressof(m_data)) + index;
            }
        }

        LLEC_NODISCARD constexpr iterator iterator_adapter(auto it)
        {
            if constexpr (std::is_same_v<decltype(it), const_iterator>)
            {
                return iterator{const_cast<typename iterator::pointer>(std::addressof(*it))};
            }
            else
            {
                return it;
            }
        }

        template <typename ItType, bool Begin>
        LLEC_NODISCARD constexpr auto iterator_selector() noexcept
        {
            if constexpr (std::is_same_v<ItType, iterator>)
            {
                if constexpr (Begin)
                {
                    return begin();
                }
                else
                {
                    return end();
                }
            }
            else
            {
                if constexpr (Begin)
                {
                    return cbegin();
                }
                else
                {
                    return cend();
                }
            }
        }

        constexpr void move_all(static_vector&& other) noexcept
        {
            if (this != std::addressof(other)) LLEC_LIKELY
            {
                if (m_count) LLEC_LIKELY
                {
                    clear();
                }

                for (size_type i = 0; i < other.m_count; i++)
                {
                    insert(begin() + i, std::move(*(other.begin() + i)));
                }

                other.m_count = 0;
            }
        }

      private:
        storage_type m_data{};
        size_type m_count{0};
    };
} // namespace llec
