#include "engine/platform/Paths.hpp"

#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#ifndef NOMINMAX
#define NOMINMAX
#endif
#include <windows.h>
#endif

#include <string>
#include <system_error>

namespace Engine::Platform
{
    std::filesystem::path ExecutableDir()
    {
#ifdef _WIN32
        std::wstring buffer(MAX_PATH, L'\0');
        for (;;)
        {
            const DWORD length = GetModuleFileNameW(nullptr, buffer.data(), static_cast<DWORD>(buffer.size()));
            if (length == 0)
            {
                break;
            }
            if (length < buffer.size())
            {
                buffer.resize(length);
                return std::filesystem::path(buffer).parent_path();
            }
            // Path longer than the buffer: grow and retry
            buffer.resize(buffer.size() * 2);
        }
#else
        std::error_code ec;
        const auto exe = std::filesystem::read_symlink("/proc/self/exe", ec);
        if (!ec)
        {
            return exe.parent_path();
        }
#endif
        return std::filesystem::current_path();
    }
}
