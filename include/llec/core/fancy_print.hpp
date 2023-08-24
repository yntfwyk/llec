/*
 * fancy_print.hpp
 * Provides minimalistic console debug message printing functionality.
 * Available printing functionalities: FANCY_INFO, FANCY_WARN, FANCY_ERROR
 * Example: FANCY_INFO("llec: %i", 9);
 * Outputs: "[info] function(signature)(line number): llec 9"
 */

#pragma once
#include "container/fixed_string.hpp"
#include "core/core.hpp"

namespace llec
{
    enum class LogType
    {
        LOG_ERR,
        LOG_WARN,
        LOG_INFO
    };

    namespace details
    {
        template <typename... Ts>
        inline void print_impl(const char* message, Ts&&... params) noexcept
        {
            LLEC_ASSERT(message);
            printf(message, params...);
        }

        template <>
        inline void print_impl<>(const char* message) noexcept
        {
            LLEC_ASSERT(message);
            printf("%s", message);
        }

        template <LogType TLog>
        LLEC_NODISCARD constexpr fixed_string1024 message_builder(const char* func, const char* line) noexcept
        {
            constexpr auto logCode = []() constexpr -> fixed_string1024 {
                if constexpr (TLog == LogType::LOG_ERR)
                {
                    return fixed_string1024("[error] ");
                }
                else if constexpr (TLog == LogType::LOG_WARN)
                {
                    return fixed_string1024("[warning] ");
                }
                else
                {
                    return fixed_string1024("[info] ");
                }
            };
            auto test = logCode();
            return test.append(func).append("(").append(line).append("): ");
        }

        template <LogType TLog, typename... Ts>
        inline void fancy_print(const char* func, const char* line, const char* message, Ts&&... params) noexcept // basic printing
        {
            LLEC_ASSERT(message);
            LLEC_ASSERT(func);
            LLEC_ASSERT(line);
            details::print_impl(details::message_builder<TLog>(func, line).append(message).append("\n").data(), std::forward<Ts>(params)...);
        }

    } // namespace details
} // namespace llec

#define STRINGIFY2(x) #x
#define STRINGIFY(x) STRINGIFY2(x)
#if defined(LLEC_PLATFORM_WINDOWS)
#define FANCY_ERROR(message, ...) llec::details::fancy_print<llec::LogType::LOG_ERR>(__FUNCSIG__, STRINGIFY(__LINE__), message, __VA_ARGS__)
#define FANCY_WARN(message, ...) llec::details::fancy_print<llec::LogType::LOG_WARN>(__FUNCSIG__, STRINGIFY(__LINE__), message, __VA_ARGS__)
#define FANCY_INFO(message, ...) llec::details::fancy_print<llec::LogType::LOG_INFO>(__FUNCSIG__, STRINGIFY(__LINE__), message, __VA_ARGS__)
#elif defined(LLEC_PLATFORM_LINUX) || defined(LLEC_PLATFORM_APPLE)
#define FANCY_ERROR(message, ...) llec::details::fancy_print<llec::LogType::LOG_ERR>(__PRETTY_FUNCTION__, STRINGIFY(__LINE__), message, __VA_ARGS__)
#define FANCY_WARN(message, ...) llec::details::fancy_print<llec::LogType::LOG_WARN>(__PRETTY_FUNCTION__, STRINGIFY(__LINE__), message, __VA_ARGS__)
#define FANCY_INFO(message, ...) llec::details::fancy_print<llec::LogType::LOG_INFO>(__PRETTY_FUNCTION__, STRINGIFY(__LINE__), message, __VA_ARGS__)
#endif
