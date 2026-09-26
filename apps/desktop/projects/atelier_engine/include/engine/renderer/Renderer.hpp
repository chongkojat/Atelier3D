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
        // Position is relative to the default spot 3 units in front of the camera
        void DrawCube(float angleDegrees, float x = 0.0f, float y = 0.0f, float z = 0.0f) const;
        void DrawPyramid(float angleDegrees, float x = 0.0f, float y = 0.0f, float z = 0.0f) const;
    };
}
