#pragma once

#include "Editor/Panels/Panel.hpp"

namespace Editor::Panels
{
    class GameViewPanel final : public Panel
    {
    public:
        GameViewPanel() : Panel("GameView") {}
        void Draw() override {}
    };
}
