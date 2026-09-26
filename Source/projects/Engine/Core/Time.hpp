#pragma once

#include <chrono>

namespace Engine::Core
{
    class Time
    {
    public:
        using Clock = std::chrono::steady_clock;

        static Clock::time_point Now() { return Clock::now(); }

        static double SecondsBetween(Clock::time_point a, Clock::time_point b)
        {
            return std::chrono::duration<double>(b - a).count();
        }
    };
}
