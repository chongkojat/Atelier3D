#pragma once

#include "editor/panels/Panel.hpp"

namespace Editor::Panels
{
    class InspectorPanel final : public Panel
    {
    public:
        InspectorPanel() : Panel("Inspector") {}
        void Draw() override {}
    };
}
