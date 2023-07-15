/*
 * time_capture.hpp
 * A minimalistic timer class.
 */

#pragma once

#include "core.hpp"
#if defined(LLEC_PLATFORM_WINDOWS) && !defined(LLEC_USE_CHRONO)
#include <Windows.h>
#else
#include <chrono>
#endif

namespace llec
{
    enum class TimeResolution : u64
    {
        Milli = 1'000ull,
        Micro = 1'000'000ull,
        Nano = 1'000'000'000ull
    };

    /// @brief Basic time capture class, acts like a stopwatch.
    /// Can be used for timing functions/pieces of code.
    /// @tparam Scoped If 'true', the start method gets called on creation otherwise have to call start method
    /// manually. The stop method has to be called to get the recorded time.
    template <bool Scoped>
    class time_capture
    {
#ifdef LLEC_PLATFORM_WINDOWS
        using counter_type = __int64;
#else
        using counter_type = std::chrono::time_point<std::chrono::high_resolution_clock>;
#endif // LLEC_PLATFORM_WINDOWS

      public:
        /// @brief Default constructor, initalizes system frequency on creation on Windows.
        /// Trivial on other platforms.
        time_capture() noexcept
        {
#ifdef LLEC_PLATFORM_WINDOWS
            QueryPerformanceFrequency(&m_frequency);
#endif // LLEC_PLATFORM_WINDOWS
            if constexpr (Scoped)
            {
                start();
            }
        }
        ~time_capture() noexcept = default;
        time_capture(const time_capture&) = delete;
        time_capture(time_capture&&) = delete;
        time_capture operator=(const time_capture&) = delete;
        time_capture operator=(time_capture&&) = delete;

        /// @brief Starts recording time.
        void start() noexcept
        {
#ifdef LLEC_PLATFORM_WINDOWS
            LARGE_INTEGER largeInt{};
            QueryPerformanceCounter(&largeInt);
            m_start = largeInt.QuadPart;
#else
            m_Start = std::chrono::high_resolution_clock::now();
#endif // LLEC_PLATFORM_WINDOWS
        }

        /// @brief Stop recording time and returns the elapsed time in the specified resolution.
        /// @tparam resolution By default set to TimeResolution::Milli (only Milli, Micro and Nano supported for now)
        /// @return time elapsed
        template <TimeResolution resolution = TimeResolution::Milli>
        LLEC_NODISCARD double stop() const noexcept
        {
#ifdef LLEC_PLATFORM_WINDOWS
            LARGE_INTEGER largeInt;
            QueryPerformanceCounter(&largeInt);
            const double timeElapsed =
                (largeInt.QuadPart - m_start) * static_cast<double>(resolution) / m_frequency.QuadPart;
#else
            using duration_type = std::conditional_t<
                resolution == TimeResolution::Milli, std::chrono::duration<double, std::milli>,
                std::conditional_t<
                    resolution == TimeResolution::Micro, std::chrono::duration<double, std::micro>,
                    std::conditional_t<resolution == TimeResolution::Nano, std::chrono::duration<double, std::nano>,
                                       std::chrono::duration<double, std::milli>>>>;

            const double timeElapsed = (duration_type(std::chrono::high_resolution_clock::now() - m_Start)).count();
#endif // LLEC_PLATFORM_WINDOWS
            return timeElapsed;
        }

      private:
        counter_type m_start{};
#ifdef LLEC_PLATFORM_WINDOWS
        LARGE_INTEGER m_frequency{};
#endif // LLEC_PLATFORM_WINDOWS
    };

    /// @brief Scoped timer, have to call stop method manually.
    using scoped_time_capture = time_capture<true>;

    /// @brief Unscoped timer, have to call both start and stop manually.
    using unscoped_time_capture = time_capture<false>;
} // namespace llec
