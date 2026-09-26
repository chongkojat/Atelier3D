#pragma once

#include "editor/panels/Panel.hpp"

namespace Editor::Panels
{
    class SceneViewPanel final : public Panel
    {
    public:
        SceneViewPanel() : Panel("SceneView") {}
        void Draw() override {}
    };
}
