#pragma once

#include "engine/platform/Window.hpp"

#include <cstdint>

namespace Engine::ImGuiLayer
{
    class ImGuiLayer
    {
    public:
        void Init(Platform::Window& window);
        void Shutdown();

        void BeginFrame(Platform::Window& window, double deltaSeconds);
        void EndFrame();
    };

#ifdef _WIN32
    bool HandleWin32Message(void* hwnd, unsigned int msg, std::uintptr_t wParam, std::intptr_t lParam);
#endif
}
