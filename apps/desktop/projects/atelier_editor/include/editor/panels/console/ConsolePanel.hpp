#pragma once

#include "editor/panels/Panel.hpp"

namespace Editor::Panels
{
    class ConsolePanel final : public Panel
    {
    public:
        ConsolePanel() : Panel("Console") {}
        void Draw() override {}
    };
}
