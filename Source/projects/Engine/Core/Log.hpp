#pragma once

#include <iostream>
#include <string_view>

namespace Engine::Core
{
    enum class LogLevel
    {
        Info,
        Warning,
        Error,
    };

    inline void Log(LogLevel level, std::string_view message)
    {
        switch (level)
        {
        case LogLevel::Info: std::cout << "[Info] "; break;
        case LogLevel::Warning: std::cout << "[Warn] "; break;
        case LogLevel::Error: std::cerr << "[Error] "; break;
        }

        if (level == LogLevel::Error)
        {
            std::cerr << message << "\n";
        }
        else
        {
            std::cout << message << "\n";
        }
    }
}
