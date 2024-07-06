#pragma once

#include <variant>

#include "core/core.hpp"
#include "private/aligned_storage.hpp"
#include "utility/function_traits.hpp"

namespace llec
{
    template <size_t, typename>
    class stack_function;

    template <size_t N, typename R, typename... Args>
    requires std::is_invocable_r_v<R, R (*)(Args...), Args...>
    class stack_function<N, R(Args...)>
    {
        /// @brief alignment is based on system (4 byte aligned for 32 bit and 8 byte for 64 bit systems)
        using storage_t = details::aligned_storage_t<N, sizeof(void*)>;

        using invokePtr_t = R (*)(const void*, Args&&...);
        using createPtr_t = void (*)(void*, const void*);
        using destroyPtr_t = void (*)(void*);
        using invokeFuncPtr_t = R (*)(Args...);

      public:
        using result_type = R;

        stack_function() = default;

        template <typename Func>
        stack_function(Func&& func) noexcept
        {
            if constexpr (std::is_member_function_pointer_v<Func>)
            {
                initialize(std::mem_fn(std::forward<Func>(func)));
            }
            else
            {
                initialize(std::forward<Func>(func));
            }
        }

        stack_function(std::nullptr_t) noexcept
        {
            reset();
        }

        ~stack_function() noexcept
        {
            reset();
        }

        template <typename Func>
        requires std::invocable<Func, Args...>
        stack_function& operator=(Func&& other) noexcept
        {
            if constexpr (std::is_same_v<std::decay_t<decltype(other)>, stack_function>)
            {
                if (this != std::addressof(other))
                {
                    if (auto classStorage = std::get_if<Storage>(&other.m_storage))
                    {
                        classStorage->m_destroyPtr(std::addressof(classStorage->m_data));

                        m_storage.template emplace<Storage>(*classStorage);
                    }
                    else if (auto fnStorage = std::get_if<invokeFuncPtr_t>(&other.m_storage))
                    {
                        m_storage.template emplace<invokeFuncPtr_t>(*fnStorage);
                    }
                }
            }
            else
            {
                initialize(std::forward<Func>(other));
            }
            return *this;
        }

        stack_function& operator=(std::nullptr_t)
        {
            reset();
        }

        R operator()(Args... args) const noexcept
        {
            if (auto callable = std::get_if<invokeFuncPtr_t>(&m_storage))
            {
                return (*callable)(std::forward<Args>(args)...);
            }
            else
            {
                auto storage = std::get_if<Storage>(&m_storage);
                return storage->m_invokePtr(std::addressof(storage->m_data), std::forward<Args>(args)...);
            }
        }

        void reset() noexcept
        {
            if (auto storagePtr = std::get_if<Storage>(&m_storage))
            {
                storagePtr->m_destroyPtr(std::addressof(storagePtr->m_data));
            }

            m_storage = std::monostate{};
        }

        explicit operator bool() const noexcept
        {
            return !std::holds_alternative<std::monostate>(m_storage);
        }

      private:
        template <typename Func>
        static R invoke(const Func* func, Args&&... args) noexcept
        {
            return (*func)(std::forward<Args>(args)...);
        }

        template <typename Func>
        static void create(Func* destination, const Func* source) noexcept
        {
            std::construct_at(destination, *source);
        }

        template <typename Func>
        static void destroy(Func* func) noexcept
        {
            std::destroy_at(func);
        }

        template <typename Func>
        void initialize(Func&& func) noexcept
        {
            static_assert(std::is_invocable_r_v<R, decltype(func), Args...>);
            if constexpr (std::is_class_v<Func>)
            {
                static_assert(sizeof(Func) <= N, "llec error: insufficient memory! try a larger size.");
                Storage& storage = m_storage.template emplace<Storage>(reinterpret_cast<createPtr_t>(create<Func>),
                                                                       reinterpret_cast<invokePtr_t>(invoke<Func>),
                                                                       reinterpret_cast<destroyPtr_t>(destroy<Func>));
                storage.m_createPtr(std::addressof(storage.m_data), std::addressof(func));
            }
            else
            {
                m_storage.template emplace<invokeFuncPtr_t>(reinterpret_cast<invokeFuncPtr_t>(func));
            }
        }

        struct Storage
        {
            createPtr_t m_createPtr{nullptr};
            invokePtr_t m_invokePtr{nullptr};
            destroyPtr_t m_destroyPtr{nullptr};
            storage_t m_data;
        };

        std::variant<std::monostate, Storage, invokeFuncPtr_t> m_storage;
    };
} // namespace llec