#pragma once

#include <cstdint>

namespace Engine::Renderer
{
    class Renderer
    {
    public:
        void Init();
        void Shutdown();
        void BeginFrame(int width, int height);
        void EndFrame();

        void SetClearColor(float r, float g, float b, float a);
        void Clear();
        void DrawCube(float angleDegrees) const;
        void DrawPyramid(float angleDegrees) const;
    };
}
