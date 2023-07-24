/*
 * fixed_vector.hpp
 * a vector on the stack, almost every function is marked constexpr
 * so we can expect compile time evaluations.
 * not all functionality is present from std::vector, though the crucial and important ones are implemented.
 * this implementation is based of David Stone's implementation.
 */

#pragma once
#include "../core/core.hpp"
#include "../core/memory.hpp"
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
    class fixed_vector
    {
        static_assert(Capacity > 0);
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

        constexpr fixed_vector() noexcept = default;
        ~fixed_vector() noexcept
            requires(std::is_trivially_destructible_v<T>)
        = default;

        constexpr ~fixed_vector() noexcept
        {
            clear();
        }

        constexpr fixed_vector(std::initializer_list<value_type> il) noexcept
        {
            insert(cbegin(), il.begin(), il.end());
        }

        fixed_vector(const fixed_vector&) noexcept
            requires(std::is_trivially_copy_constructible_v<value_type>)
        = default;

        constexpr fixed_vector(const fixed_vector& other) noexcept
        {
            copy_all(other);
        }

        fixed_vector& operator=(const fixed_vector&) noexcept
            requires(std::is_trivially_copy_assignable_v<value_type>)
        = default;

        constexpr fixed_vector& operator=(const fixed_vector& other) noexcept
        {
            copy_all(other);
            return *this;
        }

        fixed_vector(fixed_vector&&) noexcept
            requires(std::is_trivially_move_constructible_v<value_type>)
        = default;

        constexpr fixed_vector(fixed_vector&& other) noexcept
        {
            if (this != std::addressof(other))
                LLEC_LIKELY
                {
                    relocate_all(std::move(other));
                }
        }

        fixed_vector& operator=(fixed_vector&&) noexcept
            requires(std::is_trivially_move_assignable_v<value_type>)
        = default;

        constexpr fixed_vector& operator=(fixed_vector&& other) noexcept
        {
            if (this != std::addressof(other))
                LLEC_LIKELY
                {
                    // once destroyed, the memory is uninitialized that means we can do safe relocations
                    clear();
                    relocate_all(std::move(other));
                }
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
        /// @tparam ConstIt
        /// @tparam ...Args
        /// @param pos
        /// @param ...args
        /// @return
        template <typename ConstIt, typename... Args>
        iterator emplace(ConstIt pos, Args&&... args) noexcept
            requires std::is_same_v<ConstIt, iterator> || std::is_same_v<ConstIt, const_iterator>
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
                if constexpr (traits::trivially_relocatable<value_type>)
                {
                    std::copy_backward(iterator_adapter(pos), end(), end() + 1);
                }
                else
                {
                    memory::relocate_backward(iterator_adapter(pos), end(), end() + 1);
                }
                std::construct_at(std::addressof(*_pos), std::forward<Args>(args)...);
            }

            m_count++;
            if constexpr (std::is_same_v<ConstIt, const_iterator>)
                return iterator{const_cast<typename iterator::pointer>(std::addressof(*_pos))};
            else
                return pos;
        }

        /// @brief inserts data at the specified position in the vector
        /// @tparam ConstIt
        /// @param pos
        /// @param value
        /// @return
        template <typename ConstIt>
        constexpr iterator insert(ConstIt pos, const value_type& value) noexcept
            requires std::is_same_v<ConstIt, iterator> || std::is_same_v<ConstIt, const_iterator>
        {
            return emplace(pos, value);
        }

        /// @brief inserts data at the specified position in the vector
        /// @tparam ConstIt
        /// @param pos
        /// @param value
        /// @return
        template <typename ConstIt>
        constexpr iterator insert(ConstIt pos, value_type&& value) noexcept
            requires std::is_same_v<ConstIt, iterator> || std::is_same_v<ConstIt, const_iterator>
        {
            return emplace(pos, std::move(value));
        }

        /// @brief inserts range of data at the specified position in the vector
        /// @tparam ConstIt
        /// @tparam InputIt
        /// @param pos
        /// @param ibegin
        /// @param iend
        /// @return
        template <typename ConstIt, typename InputIt>
        constexpr iterator insert(ConstIt pos, InputIt ibegin, InputIt iend) noexcept
            requires std::is_same_v<ConstIt, iterator> || std::is_same_v<ConstIt, const_iterator>
        {
            LLEC_ASSERT(ibegin < iend);
            LLEC_ASSERT(m_count < Capacity);

            const size_type count = std::distance(ibegin, iend);
            LLEC_ASSERT(count <= (Capacity - m_count));

            const_iterator _pos{std::addressof(*pos)};
            LLEC_ASSERT(_pos <= cend());

            if (m_count == 0)
            {
                memory::uninitialized_copy(ibegin, iend, iterator_adapter(_pos));
            }
            else if (_pos == cend())
            {
                memory::uninitialized_copy(ibegin, iend, end());
            }
            else
            {
                if constexpr (traits::trivially_relocatable<value_type>)
                {
                    std::memmove(const_cast<typename iterator::pointer>(std::addressof(*(_pos + count))),
                                 std::addressof(*_pos), std::distance(_pos, cend()) * sizeof(value_type));
                }
                else
                {
                    memory::relocate_backward(iterator_adapter(pos), end(), end() + count);
                }
                for (size_type i = 0; i < count; i++)
                    std::construct_at(std::addressof(*(_pos + i)), *(ibegin + i));
            }

            m_count += count;
            if constexpr (std::is_same_v<ConstIt, const_iterator>)
                return iterator{const_cast<typename iterator::pointer>(std::addressof(*_pos))};
            else
                return pos;
        }

        /// @brief pops off data from the back of the vector
        constexpr void pop_back() noexcept
        {
            LLEC_ASSERT(m_count > 0 && m_count <= Capacity);
            std::destroy_at(get_data_address(--m_count));
        }

        /// @brief removes data from the specified position in the vector
        /// @tparam It
        /// @param pos
        /// @return
        template <typename It>
        constexpr iterator erase(It pos) noexcept
            requires std::is_same_v<It, iterator> || std::is_same_v<It, const_iterator>
        {
            LLEC_ASSERT(m_count > 0);
            It itEnd = iterator_selector<It, false>();
            LLEC_ASSERT(pos < itEnd);
            std::destroy_at(std::addressof(*pos));
            m_count--;
            if (pos < (itEnd - 1))
            {
                if constexpr (traits::trivially_relocatable<value_type>)
                {
                    memory::memcpy(std::addressof(*iterator_adapter(pos)), std::addressof(*(pos + 1)),
                                   std::distance((pos + 1), itEnd) * sizeof(value_type));
                }
                else
                {
                    memory::relocate(iterator_adapter(pos + 1), iterator_adapter(itEnd), iterator_adapter(pos));
                }
                return iterator_adapter(pos);
            }
            return end();
        }

        /// @brief removes range of data
        /// @tparam It
        /// @param ibegin
        /// @param iend
        /// @return
        template <typename It>
        constexpr iterator erase(It ibegin, It iend) noexcept
            requires std::is_same_v<It, iterator> || std::is_same_v<It, const_iterator>
        {
            LLEC_ASSERT(ibegin < iend);

            if (ibegin == iend || m_count == 0)
                LLEC_UNLIKELY
            return iterator_adapter(iend);

            const size_type count = std::distance(ibegin, iend);
            LLEC_ASSERT(count <= m_count);

            It itEnd = iterator_selector<It, false>();
            m_count -= count;
            std::destroy_n(ibegin, count);
            if (iend == itEnd)
                return end();

            auto first = iterator_adapter(ibegin);
            if constexpr (traits::trivially_relocatable<value_type>)
            {
                // TODO: write a constexpr version if possible
                std::memmove(std::addressof(*first), std::addressof(*iend),
                             std::distance(iend, itEnd) * sizeof(value_type));
            }
            else
            {
                memory::relocate(iterator_adapter(iend), iterator_adapter(itEnd), first);
            }
            return first;
        }

        /// @brief clears the vector i.e. all elements will destroyed and size will be set to 0
        void clear() noexcept
        {
            if constexpr (!std::is_trivially_destructible_v<T>)
            {
                std::destroy(begin(), end());
            }
            m_count = 0;
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
                return (m_data.data() + index);
            else
                return reinterpret_cast<T*>(std::addressof(m_data)) + index;
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

        LLEC_NODISCARD constexpr iterator iterator_adapter(auto it) noexcept
        {
            if constexpr (std::is_same_v<decltype(it), const_iterator>)
                return iterator{const_cast<typename iterator::pointer>(std::addressof(*it))};
            else
                return it;
        }

        template <typename ItType, bool Begin>
        LLEC_NODISCARD constexpr auto iterator_selector() noexcept
        {
            if constexpr (std::is_same_v<ItType, iterator>)
            {
                if constexpr (Begin)
                    return begin();
                else
                    return end();
            }
            else
            {
                if constexpr (Begin)
                    return cbegin();
                else
                    return cend();
            }
        }

        constexpr void copy_all(const fixed_vector& other) noexcept
        {
            if (this != std::addressof(other))
                LLEC_LIKELY
                {
                    memory::uninitialized_copy(other.cbegin(), other.cend(), begin());
                    m_count = other.m_count;
                }
        }

        constexpr void relocate_all(fixed_vector&& other) noexcept
        {
            if constexpr (traits::relocatable<value_type>)
            {
                memory::relocate(other.begin(), other.end(), begin());
                // relocation already moves and destroys so we can set other's count to zero
                m_count = std::exchange(other.m_count, 0);
            }
            else
            {
                memory::uninitialized_move(other.begin(), other.end(), begin());
                // not setting other's m_count to zero so that other's destructor
                // can destroy all the elements
                m_count = other.m_count;
            }
        }

      private:
        storage_type m_data{};
        size_type m_count{0};
    };
} // namespace llec
