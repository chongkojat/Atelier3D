#pragma once

#include "editor/panels/Panel.hpp"

namespace Editor::Panels
{
    class HierarchyPanel final : public Panel
    {
    public:
        HierarchyPanel() : Panel("Hierarchy") {}
        void Draw() override {}
    };
}
