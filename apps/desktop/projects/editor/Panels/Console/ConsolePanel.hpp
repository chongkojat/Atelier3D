#pragma once

#include "Editor/Panels/Panel.hpp"

namespace Editor::Panels
{
    class ConsolePanel final : public Panel
    {
    public:
        ConsolePanel() : Panel("Console") {}
        void Draw() override {}
    };
}
