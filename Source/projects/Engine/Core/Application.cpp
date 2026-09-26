#include "Engine/Core/Application.hpp"

#include "Engine/Core/Time.hpp"

namespace Engine::Core
{
    Application::Application(ApplicationSpec spec)
        : spec_(std::move(spec)), window_(Platform::CreateWindow(spec_.window))
    {
    }

    Application::~Application() = default;

    void Application::Run()
    {
        OnInit();

        auto last = Time::Now();

        while (running_ && !window_->ShouldClose())
        {
            const auto now = Time::Now();
            const double dt = Time::SecondsBetween(last, now);
            last = now;

            OnUpdate(dt);
            window_->Present();

            window_->PollEvents();

            ++frameCount_;
            if (spec_.maxFrames > 0 && frameCount_ >= spec_.maxFrames)
            {
                Close();
            }
        }

        // Called here rather than in ~Application(), where the derived override would no longer be reachable
        OnShutdown();
    }

    void Application::Close()
    {
        running_ = false;
        if (window_)
        {
            window_->RequestClose();
        }
    }

    Platform::Window& Application::GetWindow()
    {
        return *window_;
    }

    const Platform::Window& Application::GetWindow() const
    {
        return *window_;
    }
}
