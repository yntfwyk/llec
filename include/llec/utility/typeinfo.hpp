#pragma once
#include "container/fixed_string.hpp"
#include "core/core.hpp"
#include "utility/checksum.hpp"

namespace llec
{
    class typeinfo
    {
      public:
        template <typename T>
        static consteval auto type_name() noexcept
        {
#if defined(LLEC_COMPILER_GCC)
            constexpr std::string_view signature(__PRETTY_FUNCTION__);
            constexpr std::string_view withT{"[with T = "};
            constexpr auto pos = signature.find(withT);
            constexpr auto end = signature.find(";", pos + withT.length());
            return signature.substr(pos + withT.length(), end - pos - withT.length());
#elif defined(LLEC_COMPILER_CLANG)
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

        template <typename T>
        LLEC_NODISCARD static consteval u32 type_hash32() noexcept
        {
            return checksum::fnv1a(type_name<T>());
        }

        template <typename T>
        LLEC_NODISCARD static consteval u64 type_hash64() noexcept
        {
            return checksum::fnv1a<u64>(type_name<T>());
        }
    };

} // namespace llec