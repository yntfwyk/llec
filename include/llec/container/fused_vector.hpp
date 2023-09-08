/*
 * fused_vector.hpp
 * A sugar-coated wrapper around a collection vector-like containers.
 */

#pragma once
#include "container/fixed_vector.hpp"
#include "core/core.hpp"
#include <span>
#include <tuple>

namespace llec
{
    namespace details
    {
        template <typename T>
        concept has_tag_member = requires { typename T::container_tag; };
    }

    /// @brief helper tag to be used to define your own FVContainer type, see fusedvector_tests.cc for examples
    struct fused_vector_container_tag;

    /// @brief A wrapper around collection of vector-like containers. Keeps the addition and deletion of elements in
    /// sync.
    /// @tparam FVContainer User defined FVContainer type, see fusedvector_tests.cc for example. Let users use the own
    /// vector-like containers as an underlying data structure.
    /// @tparam ...Types Component types, Note: No duplicate types are supported
    template <typename FVContainer, typename... Types>
    requires details::has_tag_member<FVContainer>
    class fused_vector
    {
        static constexpr std::size_t N = sizeof...(Types);
        template <typename T>
        using component_container = typename FVContainer::template container_type<T>;
        using fused_containers_t = std::tuple<component_container<Types>...>;
        static_assert(N > 1, "llec error: template arguments can not be less than two!");
        static_assert(traits::is_vector_like<std::tuple_element_t<0, fused_containers_t>>,
                      "llec error: template argument FVContainer must be a vector-like type!");
        static_assert(std::is_same_v<typename FVContainer::container_tag, fused_vector_container_tag>,
                      "llec error: container not compatible see fusedvector_tests.cc for examples!");

      public:
        /// @brief returns the number of components specified in compile-time
        /// @return
        LLEC_NODISCARD constexpr std::size_t get_component_count() const noexcept
        {
            return N;
        }

        /// @brief reserves memory for number of elements specified
        /// @param count
        constexpr void reserve(std::size_t count) noexcept
        {
            std::apply([count](auto&... v) { (v.reserve(count), ...); }, m_fusedContainers);
        }

        /// @brief resizes containers to the specified amount
        /// @param count
        constexpr void resize(std::size_t count) noexcept
        {
            std::apply([count](auto&... v) { (v.resize(count), ...); }, m_fusedContainers);
        }

        /// @brief inserts values to their respective containers
        /// @tparam ...Args
        /// @param ...args values to insert
        template <typename... Args>
        requires(sizeof...(Args) == N)
        constexpr void push_back(Args&&... args) noexcept
        {
            (std::get<component_container<std::remove_cv_t<Args>>>(m_fusedContainers)
                 .push_back(std::forward<Args>(args)),
             ...);
        }

        /// @brief removes an element at the specified index
        /// @param index
        constexpr void erase(std::size_t index) noexcept
        {
            std::apply([index](auto&... v) { (v.erase(v.begin() + index), ...); }, m_fusedContainers);
        }

        /// @brief range removes elements in the given range. [first, last)
        /// @param first start index
        /// @param last end index
        constexpr void erase(std::size_t first, std::size_t last) noexcept
        {
            LLEC_ASSERT(first >= 0);
            LLEC_ASSERT(last < size());
            LLEC_ASSERT(first < last);

            std::apply([first, last](auto&... v) { (v.erase(v.begin() + first, v.begin() + last), ...); },
                       m_fusedContainers);
        }

        /// @brief removes all the elements
        constexpr void clear() noexcept
        {
            std::apply([](auto&... v) { (v.clear(), ...); }, m_fusedContainers);
        }

        /// @brief move a container to the specified component container
        /// @tparam T component type
        /// @tparam Container
        /// @param container container to move from
        template <typename T, typename Container>
        requires traits::is_vector_like<Container>
        constexpr void move_component_data(Container&& container) noexcept
        {
            static_assert(std::is_rvalue_reference_v<decltype(container)>, "llec error: expecting an rvalue!");
            LLEC_ASSERT(size() == container.size());
            std::get<component_container<std::remove_cv_t<T>>>(m_fusedContainers) = std::move(container);
        }

        /// @brief copy from a container to the specified componenet container
        /// @tparam T component type
        /// @tparam Container
        /// @param container container to copy from
        template <typename T, typename Container>
        requires traits::is_vector_like<Container>
        constexpr void copy_component_data(const Container& container) noexcept
        {
            LLEC_ASSERT(size() == container.size());
            std::get<component_container<std::remove_cv_t<T>>>(m_fusedContainers) = container;
        }

        /// @brief returns reference to mutable data from a specified component container at a specified index
        template <typename T>
        LLEC_NODISCARD constexpr auto& get_component_at(std::size_t index) noexcept
        {
            LLEC_ASSERT(index < size());
            return std::get<component_container<std::remove_cv_t<T>>>(m_fusedContainers)[index];
        }

        /// @brief returns reference to immutable data from a specified component container at a specified index
        template <typename T>
        LLEC_NODISCARD constexpr const auto& get_component_at(std::size_t index) const noexcept
        {
            LLEC_ASSERT(index < size());
            return std::get<component_container<std::remove_cv_t<T>>>(m_fusedContainers)[index];
        }

        /// @brief returns number of elements, all component containers are in sync so their sizes are equal
        /// @return
        LLEC_NODISCARD constexpr std::size_t size() const noexcept
        {
            return std::get<0>(m_fusedContainers).size();
        }

        /// @brief returns a mutable span of a specific component container
        /// @tparam T component type
        /// @return
        template <typename T>
        LLEC_NODISCARD constexpr std::span<std::remove_cv_t<T>> component_view() noexcept
        {
            return {std::get<component_container<std::remove_cv_t<T>>>(m_fusedContainers).data(), size()};
        }

        /// @brief returns a immutable span of a specific component container
        /// @tparam T component type
        /// @return
        template <typename T>
        LLEC_NODISCARD constexpr const std::span<std::remove_cv_t<T>> component_view() const noexcept
        {
            return {std::get<component_container<std::remove_cv_t<T>>>(m_fusedContainers).data(), size()};
        }

      private:
        fused_containers_t m_fusedContainers;
    };
} // namespace llec