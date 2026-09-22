#include "Engine/Platform/NullWindow.hpp"

#ifdef _WIN32
#include "Engine/Platform/WindowsWindow.hpp"
#endif

namespace Engine::Platform
{
    NullWindow::NullWindow(WindowSpec spec) : spec_(std::move(spec)) {}

    void NullWindow::PollEvents() {}
    void NullWindow::Present() {}

    bool NullWindow::ShouldClose() const
    {
        return shouldClose_;
    }

    void NullWindow::RequestClose()
    {
        shouldClose_ = true;
    }

    int NullWindow::Width() const
    {
        return spec_.width;
    }

    int NullWindow::Height() const
    {
        return spec_.height;
    }

    const std::string& NullWindow::Title() const
    {
        return spec_.title;
    }

    std::unique_ptr<Window> CreateWindow(const WindowSpec& spec)
    {
#ifdef _WIN32
        return std::make_unique<WindowsWindow>(spec);
#else
        return std::make_unique<NullWindow>(spec);
#endif
    }
}
