#pragma once

#include <filesystem>

namespace Engine::Platform
{
    // Folder containing the running executable, whatever folder it was launched from
    // (falls back to the working directory if the OS cannot report it)
    std::filesystem::path ExecutableDir();
}
