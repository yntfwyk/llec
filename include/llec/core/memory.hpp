/*
 * memory.hpp
 * collection of helper constexpr memory manipulation functions.
 */

#pragma once
#include "../core/core.hpp"

namespace llec::memory
{

    /// @brief constexpr memcpy
    /// @param dest
    /// @param src
    /// @param n
    /// @return
    constexpr void* memcpy(void* dest, const void* src, std::size_t n) noexcept
    {
        if (std::is_constant_evaluated())
        {
            const std::byte* srcBytes = static_cast<const std::byte*>(src);
            std::byte* destBytes = static_cast<std::byte*>(dest);
#ifdef LLEC_COMPILER_CLANG
#pragma clang loop vectorize(assume_safety)
#elif defined(LLEC_COMPILER_GCC)
#pragma GCC ivdep
#endif // LLEC_COMPILER_CLANG
            for (std::size_t i = 0; i < n; i++)
                destBytes[i] = srcBytes[i];

            return destBytes;
        }
        else
        {
            return std::memcpy(dest, src, n);
        }
    }

    /// @brief constexpr uninitialized_copy
    /// @tparam InputIt
    /// @tparam OutputIt
    /// @param first
    /// @param last
    /// @param destBegin
    /// @return
    template <class InputIt, class OutputIt>
    constexpr OutputIt uninitialized_copy(InputIt first, InputIt last, OutputIt destBegin) noexcept
    {
        using T = typename std::iterator_traits<OutputIt>::value_type;
        OutputIt current = destBegin;
        for (; first != last; ++first, (void)++current)
            std::construct_at(std::addressof(*current), *first);
        return current;
    }
    
    /// @brief constexpr uninitialized_copy
    /// @tparam InputIt
    /// @tparam OutputIt
    /// @param first
    /// @param last
    /// @param destBegin
    /// @return
    template <class InputIt, class OutputIt>
    constexpr OutputIt uninitialized_move(InputIt first, InputIt last, OutputIt destBegin) noexcept
    {
        using T = typename std::iterator_traits<OutputIt>::value_type;
        OutputIt current = destBegin;
        for (; first != last; ++first, (void)++current)
            std::construct_at(std::addressof(*current), std::move(*first));
        return current;
    }

    /// @brief relocation, i.e. move + delete/destroy
    /// @tparam T
    /// @param destination
    /// @param source
    /// @return
    template <typename T>
    constexpr T* relocate(T* destination, T* source) noexcept
    {
        struct guard
        {
            T* t;
            ~guard()
            {
                std::destroy_at(t);
            }
        } g{source};
        return std::construct_at(destination, std::move(*source));
    }

    /// @brief relocation support for forward iterators
    /// @tparam It1
    /// @tparam It2
    /// @param first
    /// @param last
    /// @param destinationFirst
    template <typename It1, typename It2>
    constexpr void relocate(It1 first, It1 last, It2 destinationFirst) noexcept
    {
        for (; first != last; ++first, (void)++destinationFirst)
        {
            relocate(std::addressof(*destinationFirst), std::addressof(*first));
        }
    }

    /// @brief relocation but backwards
    /// @tparam It1
    /// @tparam It2
    /// @param first
    /// @param last
    /// @param destinationFirst
    template <typename It1, typename It2>
    constexpr void relocate_backward(It1 first, It1 last, It2 destinationFirst) noexcept
    {
        for (; first != last;)
        {
            relocate(std::addressof(*(--destinationFirst)), std::addressof(*(--last)));
        }
    }

} // namespace llec::memory