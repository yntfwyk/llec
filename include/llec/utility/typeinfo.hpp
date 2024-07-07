#pragma once
#include "core/core.hpp"
#include "utility/checksum.hpp"

namespace llec
{
    class typeinfo
    {
      public:
        /// @brief Evaluates name of specified type in compile time
        /// @tparam T Type to get the name of
        /// @return std::string_view
        template <typename T>
        static consteval auto type_name() noexcept
        {
#if defined(LLEC_COMPILER_CLANG) || defined(LLEC_COMPILER_GCC)
            constexpr std::string_view signature(__PRETTY_FUNCTION__);
            constexpr std::string_view withT{"T = "};
            constexpr auto pos = signature.find(withT);
            constexpr auto end = signature.find("]", pos + withT.length());
            return signature.substr(pos + withT.length(), end - pos - withT.length());
#elif defined(LLEC_COMPILER_MSVC)
            constexpr std::string_view signature(__FUNCSIG__);
            constexpr std::string_view exclude{__func__};
            constexpr auto start = signature.find(exclude);
            constexpr auto offset = start + exclude.length() + 1;
            constexpr auto end = signature.find(">(void) noexcept");

            return signature.substr(offset, end - offset);
#else
#error compiler not supported
#endif
        }

        /// @brief Returns a u32 fnv1a hash of a type in compile time
        /// @tparam T Type to get the hash of
        template <typename T>
        LLEC_NODISCARD static consteval u32 type_hash32() noexcept
        {
            return checksum::fnv1a(type_name<T>());
        }

        /// @brief Returns a u64 fnv1a hash of a type in compile time
        /// @tparam T Type to get the hash of
        template <typename T>
        LLEC_NODISCARD static consteval u64 type_hash64() noexcept
        {
            return checksum::fnv1a<u64>(type_name<T>());
        }
    };

} // namespace llec