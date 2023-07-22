/*
 * index_preservation_array.hpp
 * commonly known as slot_map or SlotMap, the name was changed here because a Map
 * is a data structure that takes in a Key/Value pair, whereas this data structure generates a key
 * for the user. This version does not allocate any heap memory and is static in nature.
 * Thanks to Professor Retroman(@ProfesorRetroman) and Allan Deutsch(Allan@allandeutsch.com)
 */

#pragma once
#include "private/aligned_storage.hpp"
#include <array>

namespace llec
{
    /// @brief index preservation array, or usually known as slot_map.
    /// This class could have been implemented using static_vector but the decision was not to add that dependency
    /// @tparam T type
    /// @tparam Capacity maximum number of elements
    /// @tParam TSize type of index, if dealing smalled quantity of data can use u8, u16 etc.
    template <typename T, std::size_t Capacity, typename TSize = std::size_t>
    class index_preservation_array
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

        struct handle
        {
            size_type id{0};
            size_type generation{0};
        };

        index_preservation_array() noexcept
        {
            init_index_array();
        }

        // TODO: add constrcutors and destructors

        /// @brief inserts an element into the container
        /// @param value data to insert (lvalue)
        /// @return key associated with the data
        constexpr handle insert(const value_type& value) noexcept
        {
            const size_type dataIndex = make_data();
            const auto& slot = m_indices[dataIndex];
            m_erase[slot.id] = dataIndex;
            std::construct_at(get_data_address(m_count - 1), value);
            return {dataIndex, slot.generation};
        }

        /// @brief inserts an element into the container
        /// @param value data to insert (rvalue)
        /// @return key associated with the data
        constexpr handle insert(value_type&& value) noexcept
        {
            const size_type dataIndex = make_data();
            const auto& slot = m_indices[dataIndex];
            m_erase[slot.id] = dataIndex;
            std::construct_at(get_data_address(m_count - 1), std::move(value));
            return {dataIndex, slot.generation};
        }

        /// @brief removes the data from the container
        /// @param key to remove data from the container
        /// @return 'true' if data was successfully removed, else return 'false'
        constexpr bool erase(handle key) noexcept
        {
            if (!is_key_valid(key)) LLEC_UNLIKELY
                return false;

            handle& slot = m_indices[key.id];
            const size_type dataIndex = slot.id;
            std::destroy_at(get_data_address(dataIndex));
            const size_type lastIndex = m_count - 1;
            if (dataIndex != lastIndex)
            {
                std::exchange(*get_data_address(dataIndex), *get_data_address(lastIndex));
                m_erase[dataIndex] = m_erase[lastIndex];
                m_indices[m_erase[dataIndex]].id = dataIndex;
            }
            m_count--;
            slot.id = m_freeList;
            slot.generation++;
            m_freeList = key.id;
            return true;
        }

        /// @brief clears the container
        constexpr void clear() noexcept
        {
            m_generation = 0;
            m_count = 0;
            m_freeList = 0;
            init_index_array();

            if constexpr (!traits::is_trivially_xstructible_v<value_type>)
            {
                for (size_type i = 0; i < m_count; i++)
                {
                    std::destroy_at(get_data_address(i));
                }
            }
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

        /// @brief checks if the key is valid
        /// @param key
        /// @return 'true' if valid else returns 'false'
        LLEC_NODISCARD constexpr bool is_key_valid(handle key) const noexcept
        {
            return (key.id >= 0 && key.id < Capacity) && (key.generation == m_indices[key.id].generation);
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

        LLEC_NODISCARD constexpr T& operator[](handle key) noexcept
        {
            LLEC_ASSERT(is_key_valid(key));
            return *get_data_address(m_indices[key.id].id);
        }

        LLEC_NODISCARD constexpr const T& operator[](handle key) const noexcept
        {
            LLEC_ASSERT(is_key_valid(key));
            return *get_data_address(m_indices[key.id].id);
        }

      private:
        constexpr void init_index_array() noexcept
        {
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

      private:
        using storage_type =
            std::conditional_t<traits::is_trivially_xstructible_v<value_type>, std::array<value_type, Capacity>,
                               details::aligned_storage_t<sizeof(value_type) * Capacity, alignof(value_type)>>;
        storage_type m_data{};
        std::array<handle, Capacity> m_indices{};
        std::array<size_type, Capacity> m_erase{};
        size_type m_generation{0};
        size_type m_count{0};
        size_type m_freeList{0};
    };

    /// @brief type alias for index_preservation_array
    /// @tparam T type
    /// @tparam Capacity maximum number of elements
    /// @tParam TSize type of index, if dealing smalled quantity of data can use u8, u16 etc.
    template <typename T, std::size_t Capacity, typename TSize = std::size_t>
    using idxp_array = index_preservation_array<T, Capacity, TSize>;

} // namespace llec
