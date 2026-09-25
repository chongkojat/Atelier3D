#pragma once

#include <string_view>

namespace Editor::Panels
{
    class Panel
    {
    public:
        explicit Panel(std::string_view name) : name_(name) {}
        virtual ~Panel() = default;

        std::string_view Name() const { return name_; }
        virtual void Draw() {}

    private:
        std::string_view name_;
    };
}
