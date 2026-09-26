#include "Engine/Platform/WindowsWindow.hpp"

#include "Engine/ImGuiLayer/ImGuiLayer.hpp"

#include <GL/gl.h>

#include <stdexcept>
#include <string>

namespace Engine::Platform
{
    namespace
    {
        std::wstring ToWide(const std::string& utf8)
        {
            if (utf8.empty())
            {
                return {};
            }

            const int len = MultiByteToWideChar(CP_UTF8, 0, utf8.c_str(), -1, nullptr, 0);
            if (len <= 0)
            {
                return L"";
            }

            std::wstring out;
            out.resize(static_cast<size_t>(len - 1));
            MultiByteToWideChar(CP_UTF8, 0, utf8.c_str(), -1, out.data(), len);
            return out;
        }

        constexpr const wchar_t* kWindowClassName = L"GAM300_Win32Window";
    }

    WindowsWindow::WindowsWindow(WindowSpec spec) : spec_(std::move(spec))
    {
        CreateNativeWindow();
        CreateOpenGLContext();
    }

    WindowsWindow::~WindowsWindow()
    {
        if (hglrc_)
        {
            wglMakeCurrent(nullptr, nullptr);
            wglDeleteContext(hglrc_);
            hglrc_ = nullptr;
        }

        if (hdc_ && hwnd_)
        {
            ReleaseDC(hwnd_, hdc_);
            hdc_ = nullptr;
        }

        if (hwnd_)
        {
            DestroyWindow(hwnd_);
            hwnd_ = nullptr;
        }
    }

    void WindowsWindow::PollEvents()
    {
        MSG msg{};
        while (PeekMessageW(&msg, nullptr, 0, 0, PM_REMOVE))
        {
            if (msg.message == WM_QUIT)
            {
                shouldClose_ = true;
            }

            TranslateMessage(&msg);
            DispatchMessageW(&msg);
        }
    }

    void WindowsWindow::Present()
    {
        if (hdc_)
        {
            SwapBuffers(hdc_);
        }
    }

    bool WindowsWindow::ShouldClose() const
    {
        return shouldClose_;
    }

    void WindowsWindow::RequestClose()
    {
        shouldClose_ = true;
        if (hwnd_)
        {
            PostMessageW(hwnd_, WM_CLOSE, 0, 0);
        }
    }

    int WindowsWindow::Width() const
    {
        return spec_.width;
    }

    int WindowsWindow::Height() const
    {
        return spec_.height;
    }

    const std::string& WindowsWindow::Title() const
    {
        return spec_.title;
    }

    LRESULT WindowsWindow::HandleMessage(UINT msg, WPARAM wParam, LPARAM lParam)
    {
        Engine::ImGuiLayer::HandleWin32Message(hwnd_, msg, static_cast<std::uintptr_t>(wParam),
                                               static_cast<std::intptr_t>(lParam));

        switch (msg)
        {
        case WM_CLOSE:
            shouldClose_ = true;
            DestroyWindow(hwnd_);
            return 0;

        case WM_DESTROY:
            shouldClose_ = true;
            PostQuitMessage(0);
            return 0;

        case WM_SIZE:
            spec_.width = LOWORD(lParam);
            spec_.height = HIWORD(lParam);
            if (spec_.width <= 0)
            {
                spec_.width = 1;
            }
            if (spec_.height <= 0)
            {
                spec_.height = 1;
            }
            return 0;

        case WM_KEYDOWN:
            if (wParam == VK_ESCAPE)
            {
                RequestClose();
                return 0;
            }
            return 0;
        }

        return DefWindowProcW(hwnd_, msg, wParam, lParam);
    }

    LRESULT CALLBACK WindowsWindow::WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
    {
        if (msg == WM_NCCREATE)
        {
            const auto* cs = reinterpret_cast<CREATESTRUCTW*>(lParam);
            auto* window = static_cast<WindowsWindow*>(cs->lpCreateParams);
            SetWindowLongPtrW(hwnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(window));
            window->hwnd_ = hwnd;
        }

        auto* window = reinterpret_cast<WindowsWindow*>(GetWindowLongPtrW(hwnd, GWLP_USERDATA));
        if (window)
        {
            return window->HandleMessage(msg, wParam, lParam);
        }

        return DefWindowProcW(hwnd, msg, wParam, lParam);
    }

    void WindowsWindow::CreateNativeWindow()
    {
        HINSTANCE hInstance = GetModuleHandleW(nullptr);

        WNDCLASSEXW wc{};
        wc.cbSize = sizeof(WNDCLASSEXW);
        wc.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
        wc.lpfnWndProc = &WindowsWindow::WndProc;
        wc.hInstance = hInstance;
        wc.hCursor = LoadCursorW(nullptr, IDC_ARROW);
        wc.lpszClassName = kWindowClassName;

        RegisterClassExW(&wc);

        const DWORD style = WS_OVERLAPPEDWINDOW;
        RECT rect{0, 0, spec_.width, spec_.height};
        AdjustWindowRect(&rect, style, FALSE);

        const int width = rect.right - rect.left;
        const int height = rect.bottom - rect.top;

        hwnd_ = CreateWindowExW(0, kWindowClassName, ToWide(spec_.title).c_str(), style, CW_USEDEFAULT, CW_USEDEFAULT,
                                width, height, nullptr, nullptr, hInstance, this);

        if (!hwnd_)
        {
            throw std::runtime_error("CreateWindowExW failed");
        }

        ShowWindow(hwnd_, SW_SHOW);
        UpdateWindow(hwnd_);
    }

    void WindowsWindow::CreateOpenGLContext()
    {
        hdc_ = GetDC(hwnd_);
        if (!hdc_)
        {
            throw std::runtime_error("GetDC failed");
        }

        PIXELFORMATDESCRIPTOR pfd{};
        pfd.nSize = sizeof(PIXELFORMATDESCRIPTOR);
        pfd.nVersion = 1;
        pfd.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
        pfd.iPixelType = PFD_TYPE_RGBA;
        pfd.cColorBits = 32;
        pfd.cDepthBits = 24;
        pfd.cStencilBits = 8;
        pfd.iLayerType = PFD_MAIN_PLANE;

        const int pixelFormat = ChoosePixelFormat(hdc_, &pfd);
        if (pixelFormat == 0)
        {
            throw std::runtime_error("ChoosePixelFormat failed");
        }

        if (!SetPixelFormat(hdc_, pixelFormat, &pfd))
        {
            throw std::runtime_error("SetPixelFormat failed");
        }

        hglrc_ = wglCreateContext(hdc_);
        if (!hglrc_)
        {
            throw std::runtime_error("wglCreateContext failed");
        }

        if (!wglMakeCurrent(hdc_, hglrc_))
        {
            throw std::runtime_error("wglMakeCurrent failed");
        }

        glEnable(GL_DEPTH_TEST);
        glDepthFunc(GL_LEQUAL);
    }
}
