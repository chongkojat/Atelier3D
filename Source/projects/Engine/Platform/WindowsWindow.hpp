#pragma once

#include "Engine/Platform/Window.hpp"

#define WIN32_LEAN_AND_MEAN
#ifndef NOMINMAX
#define NOMINMAX
#endif
#include <windows.h>
#ifdef CreateWindow
#undef CreateWindow
#endif

namespace Engine::Platform
{
    class WindowsWindow final : public Window
    {
    public:
        explicit WindowsWindow(WindowSpec spec);
        ~WindowsWindow() override;

        void PollEvents() override;
        void Present() override;
        bool ShouldClose() const override;
        void RequestClose() override;

        int Width() const override;
        int Height() const override;
        const std::string& Title() const override;

    private:
        LRESULT HandleMessage(UINT msg, WPARAM wParam, LPARAM lParam);
        static LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

        void CreateNativeWindow();
        void CreateOpenGLContext();

        WindowSpec spec_;
        bool shouldClose_ = false;

        HWND hwnd_ = nullptr;
        HDC hdc_ = nullptr;
        HGLRC hglrc_ = nullptr;
    };
}
