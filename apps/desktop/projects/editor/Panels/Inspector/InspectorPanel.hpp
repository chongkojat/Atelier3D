#pragma once

#include "Editor/Panels/Panel.hpp"

namespace Editor::Panels
{
    class InspectorPanel final : public Panel
    {
    public:
        InspectorPanel() : Panel("Inspector") {}
        void Draw() override {}
    };
}
