#pragma once
#include "core/core.hpp"

namespace llec
{
    template<typename>
    struct function_traits;

    template<typename R, typename... Args>
    struct function_traits<R(Args...)>
    {
      private:
          using args_tuple = std::tuple<Args...>;

      public:
        using return_type = R;
        static constexpr std::size_t ARITY = sizeof...(Args);
        template<std::size_t I>
        using type_at = std::tuple_element_t<I, args_tuple>;
    };
}