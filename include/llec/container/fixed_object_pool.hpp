/*
 * fixed_object_pool.hpp
 * commonly known as slot_map or SlotMap, the name was changed here because a Map
 * is a data structure that takes in a Key/Value pair, whereas this data structure generates a handle
 * for the user. This version does not allocate any heap memory and is static in nature.
 * thanks to Professor Retroman(@ProfesorRetroman) and Allan Deutsch(Allan@allandeutsch.com)
 */

#pragma once
#include "core/memory.hpp"
#include "private/aligned_storage.hpp"
#include <array>

namespace llec
{
    /// @brief fixed memory pool, or usually known as slot_map.
    /// @tparam T type
    /// @tParam TSize type of index, if dealing smalled quantity of data can use u8, u16 etc.
    /// @tparam Capacity maximum number of elements
    template <typename T, std::size_t Capacity, typename TSize = std::size_t>
    requires std::is_same_v<std::remove_cv_t<T>, T>
    class fixed_object_pool
    {
      public:
        using value_type = T;
        using size_type = TSize;
        using difference_type = std::ptrdiff_t;
        using iterator = T*;
        using const_iterator = const T*;
        using pointer = T*;
        using const_pointer = T*;
        using reference = T&;
        using const_reference = const T&;

        struct handle_type
        {
            size_type id{};
            size_type generation{};
        };

        constexpr fixed_object_pool() noexcept
        {
            init_index_array();
        }

        ~fixed_object_pool() noexcept
        requires(std::is_trivially_destructible_v<value_type>)
        = default;

        constexpr ~fixed_object_pool() noexcept
        {
            m_generation = 0;
            m_freeList = 0;

            if constexpr (!std::is_trivially_destructible_v<value_type>)
            {
                std::destroy_n(get_data_address(), m_count);
            }

            m_count = 0;
        }

        fixed_object_pool(const fixed_object_pool&) noexcept
        requires(std::is_trivially_copy_constructible_v<value_type>)
        = default;

        constexpr fixed_object_pool(const fixed_object_pool& other) noexcept
        {
            copy_all(other);
        }

        fixed_object_pool& operator=(const fixed_object_pool&) noexcept
        requires(std::is_trivially_copy_assignable_v<value_type>)
        = default;

        constexpr fixed_object_pool& operator=(const fixed_object_pool& other) noexcept
        {
            copy_all(other);
            return *this;
        }

        fixed_object_pool(fixed_object_pool&&) noexcept
        requires(std::is_trivially_move_constructible_v<value_type>)
        = default;

        constexpr fixed_object_pool(fixed_object_pool&& other) noexcept
        {
            relocate_all(std::move(other));
        }

        fixed_object_pool& operator=(fixed_object_pool&&) noexcept
        requires(std::is_trivially_move_assignable_v<value_type>)
        = default;

        constexpr fixed_object_pool& operator=(fixed_object_pool&& other) noexcept
        {
            relocate_all(std::move(other));
            return *this;
        }

        /// @brief emplaces and element to the end of the container
        /// @tparam ...Args
        /// @param ...args
        /// @return handle associated with the data
        template <typename... Args>
        constexpr const handle_type emplace(Args&&... args) noexcept
        {
            const size_type dataIndex = make_data();
            const auto& slot = m_indices[dataIndex];
            m_erase[slot.id] = dataIndex;

            // uninitialized move
            std::construct_at(get_data_address(m_count - 1), std::forward<Args>(args)...);
            return {dataIndex, slot.generation};
        }

        /// @brief inserts an element into the container
        /// @param value data to insert (lvalue)
        /// @return handle associated with the data
        constexpr const handle_type insert(const value_type& value) noexcept
        {
            return emplace(value);
        }

        /// @brief inserts an element into the container
        /// @param value data to insert (rvalue)
        /// @return handle associated with the data
        constexpr const handle_type insert(value_type&& value) noexcept
        {
            return emplace(value);
        }

        /// @brief removes the data from the container
        /// @param handle to remove data from the container
        /// @return 'true' if data was successfully removed, else return 'false'
        constexpr bool erase(handle_type hndl) noexcept
        {
            if (!is_handle_valid(hndl))
                return false;

            handle_type& slot = m_indices[hndl.id];
            const size_type dataIndex = slot.id;
            std::destroy_at(get_data_address(dataIndex));
            const size_type lastIndex = m_count - 1;
            if (dataIndex != lastIndex)
            {
                if constexpr (traits::trivially_relocatable<value_type>)
                {
                    memory::memcpy(get_data_address(dataIndex), get_data_address(lastIndex), sizeof(value_type));
                }
                else
                {
                    memory::relocate(get_data_address(dataIndex), get_data_address(lastIndex));
                }
                m_erase[dataIndex] = m_erase[lastIndex];
                m_indices[m_erase[dataIndex]].id = dataIndex;
            }
            m_count--;
            slot.id = m_freeList;
            slot.generation++;
            m_freeList = hndl.id;
            return true;
        }

        /// @brief clears the container
        constexpr void clear() noexcept
        {
            m_generation = 0;
            m_freeList = 0;
            init_index_array();

            if constexpr (!std::is_trivially_destructible_v<value_type>)
            {
                std::destroy_n(begin(), m_count);
            }

            m_count = 0;
        }

        /// @brief returns the size of the container
        /// @return size
        LLEC_NODISCARD constexpr size_type size() const noexcept
        {
            return m_count;
        }

        /// @brief returns the capacity of the container
        /// @return capacity passed in a template argument
        LLEC_NODISCARD constexpr size_type capacity() const noexcept
        {
            return Capacity;
        }

        /// @brief checks if the handle is valid
        /// @param hndl
        /// @return 'true' if valid else returns 'false'
        LLEC_NODISCARD constexpr bool is_handle_valid(handle_type hndl) const noexcept
        {
            return (hndl.id >= 0 && hndl.id < Capacity) && (hndl.generation == m_indices[hndl.id].generation);
        }

        /// @brief returns pointer to the underlying raw data
        /// @return const pointer to raw data
        LLEC_NODISCARD constexpr const T* data() const noexcept
        {
            return get_data_address();
        }

        LLEC_NODISCARD constexpr iterator begin() noexcept
        {
            return get_data_address();
        }

        LLEC_NODISCARD constexpr iterator end() noexcept
        {
            return get_data_address(m_count);
        }

        LLEC_NODISCARD constexpr const_iterator begin() const noexcept
        {
            return get_data_address();
        }

        LLEC_NODISCARD constexpr const_iterator end() const noexcept
        {
            return get_data_address(m_count);
        }

        LLEC_NODISCARD constexpr const_iterator cbegin() const noexcept
        {
            return get_data_address();
        }

        LLEC_NODISCARD constexpr const_iterator cend() const noexcept
        {
            return get_data_address(m_count);
        }

        LLEC_NODISCARD constexpr T& operator[](handle_type hndl) noexcept
        {
            LLEC_ASSERT(is_handle_valid(hndl));
            return *get_data_address(m_indices[hndl.id].id);
        }

        LLEC_NODISCARD constexpr const T& operator[](handle_type hndl) const noexcept
        {
            LLEC_ASSERT(is_handle_valid(hndl));
            return *get_data_address(m_indices[hndl.id].id);
        }

      private:
        constexpr void init_index_array() noexcept
        {
#ifdef LLEC_COMPILER_CLANG
#pragma clang loop vectorize(assume_safety)
#elif defined(LLEC_COMPILER_GCC)
#pragma GCC ivdep
#endif // LLEC_COMPILER_CLANG
            for (size_type i = 0; i < Capacity; i++)
            {
                m_indices[i].id = i + 1;
            }
        }

        LLEC_NODISCARD constexpr size_type make_data() noexcept
        {
            LLEC_ASSERT(m_count < Capacity);
            LLEC_ASSERT(m_freeList < Capacity);

            const size_type freeSlot = m_freeList;
            m_freeList = m_indices[freeSlot].id;

            m_indices[freeSlot].id = m_count++;
            m_indices[freeSlot].generation = m_generation++;

            return freeSlot;
        }

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
                return (m_data.data() + index);
            else
                return reinterpret_cast<const T*>(std::addressof(m_data)) + index;
        }

        constexpr void copy_all(const fixed_object_pool& other) noexcept
        {
            if (this != std::addressof(other))
            {
                memory::uninitialized_copy(other.begin(), other.end(), begin());
                memory::memcpy(m_erase.data(), other.m_erase.data(), other.m_count * sizeof(size_type));
                memory::memcpy(m_indices.data(), other.m_indices.data(), capacity() * sizeof(handle_type));
                m_count = other.m_count;
                m_generation = other.m_generation;
                m_freeList = other.m_freeList;
            }
        }

        constexpr void relocate_all(fixed_object_pool&& other) noexcept
        {
            if (this != std::addressof(other))
            {
                if constexpr (traits::relocatable<value_type>)
                {
                    memory::relocate(other.begin(), other.end(), begin());
                    memory::memcpy(m_erase.data(), other.m_erase.data(), other.m_count * sizeof(size_type));
                    memory::memcpy(m_indices.data(), other.m_indices.data(), capacity() * sizeof(handle_type));
                    m_count = std::exchange(other.m_count, 0);
                    m_generation = std::exchange(other.m_generation, 0);
                    m_freeList = std::exchange(other.m_freeList, 0);
                }
                else if constexpr (traits::trivially_relocatable<value_type>)
                {
                    memory::uninitialized_move(other.begin(), other.end(), begin());
                    memory::memcpy(m_erase.data(), other.m_erase.data(), other.m_count * sizeof(size_type));
                    memory::memcpy(m_indices.data(), other.m_indices.data(), capacity() * sizeof(handle_type));
                    m_count = other.m_count;
                    m_generation = other.m_generation;
                    m_freeList = other.m_freeList;
                }
            }
        }

      private:
        using storage_type =
            std::conditional_t<traits::is_trivially_xstructible_v<value_type>, std::array<value_type, Capacity>,
                               details::aligned_storage_t<sizeof(value_type) * Capacity, alignof(value_type)>>;
        storage_type m_data;
        std::array<handle_type, Capacity> m_indices{};
        std::array<size_type, Capacity> m_erase{};
        size_type m_generation{0};
        size_type m_count{0};
        size_type m_freeList{0};
    };

    /// @brief type alias for fixed_object_pool
    /// @tparam T type
    /// @tparam Capacity maximum number of elements
    /// @tParam TSize type of index, if dealing smalled quantity of data can use u8, u16 etc.
    template <typename T, std::size_t Capacity, typename TSize = std::size_t>
    using fop = fixed_object_pool<T, Capacity, TSize>;

} // namespace llec
