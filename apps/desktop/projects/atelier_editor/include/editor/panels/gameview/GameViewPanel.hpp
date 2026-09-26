#pragma once

#include "editor/panels/Panel.hpp"

namespace Editor::Panels
{
    class GameViewPanel final : public Panel
    {
    public:
        GameViewPanel() : Panel("GameView") {}
        void Draw() override {}
    };
}
