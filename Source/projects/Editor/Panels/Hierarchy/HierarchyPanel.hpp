#pragma once

#include "Editor/Panels/Panel.hpp"

namespace Editor::Panels
{
    class HierarchyPanel final : public Panel
    {
    public:
        HierarchyPanel() : Panel("Hierarchy") {}
        void Draw() override {}
    };
}
