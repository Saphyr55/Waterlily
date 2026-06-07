#pragma once

#include "Waterlily/Core/CoreExports.hpp"
#include "Waterlily/Core/Defines.hpp"

namespace Wl
{

    /**
     * @brief Initializes the platform-specific time system.
     * This function should be called once at application startup.
     * It may initialize high-resolution timers or other platform-specific time-related resources.
     */
    WL_CORE_API void PlatformTimeInit();

    /**
     * @brief Pauses execution for a specified duration.
     * @param milliseconds The amount of time, in milliseconds, to delay execution.
     */
    WL_CORE_API void PlatformDelay(double milliseconds);

    /**
     * @brief Gets a low-resolution tick count since the system started.
     * The unit is usually milliseconds, but the resolution is platform-dependent.
     * This is generally not suitable for precise delta time calculation.
     * @return uint64_t The number of ticks (usually milliseconds) since system start.
     */
    WL_CORE_API uint64_t PlatformGetTicks();

    /**
     * @brief Gets a high-resolution, monotonically increasing performance counter value.
     * This counter is used in conjunction with @ref platform_get_performance_frequency() to calculate precise elapsed time.
     * @return uint64_t The current value of the high-resolution performance counter.
     */
    WL_CORE_API uint64_t PlatformGetPerformanceCounter();

    /**
     * @brief Gets the frequency of the performance counter.
     * This value is the number of counts per second for the counter returned by @ref platform_get_performance_counter().
     * @return uint64_t The performance counter's frequency in counts per second (Hz).
     */
    WL_CORE_API uint64_t PlatformGetPerformanceFrequency();

    /**
     * @brief Gets the current time since the application or system started.
     * The resolution may vary, but the return value is typically in seconds.
     * @return double The elapsed time in seconds.
     */
    WL_CORE_API double PlatformGetTime();

    /**
     * @brief Gets the current high-resolution time since the application or system started.
     * This uses the most precise available timer on the platform, typically based on the performance counter,
     * and returns the result in seconds.
     * @return double The high-resolution elapsed time in seconds.
     */
    WL_CORE_API double PlatformGetHighResolutionTime();

    /**
     * @brief Calculates the time difference between two performance counter values.
     * This is the preferred method for calculating delta time in a game or simulation loop.
     * @param previous_counter The performance counter value from the previous frame/update.
     * @param current_counter The performance counter value from the current frame/update.
     * @return double The elapsed time (delta time) in seconds.
     */
    WL_CORE_API double PlatformComputeDeltaTime(uint64_t previous_counter, uint64_t current_counter);

}// namespace Wl