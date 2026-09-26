#pragma once

#include "engine/platform/Window.hpp"

namespace Engine::Platform
{
    class NullWindow final : public Window
    {
    public:
        explicit NullWindow(WindowSpec spec);

        void PollEvents() override;
        void Present() override;
        bool ShouldClose() const override;
        void RequestClose() override;

        int Width() const override;
        int Height() const override;
        const std::string& Title() const override;

    private:
        WindowSpec spec_;
        bool shouldClose_ = false;
    };
}
