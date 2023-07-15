#pragma once

#include "../core/core.hpp"
#include "static_vector.hpp"

namespace llec
{
    template<typename T, std::size_t Capacity>
    class static_unordered_set
    {
      public:

      private:
        static_vector<T, Capacity> m_vector;
    };
}