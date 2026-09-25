#include "Engine/Renderer/Renderer.hpp"

#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <Windows.h>
#ifdef CreateWindow
#undef CreateWindow
#endif
#endif

#include <GL/gl.h>

#include <cmath>

namespace Engine::Renderer
{
    void Renderer::Init() {}
    void Renderer::Shutdown() {}
    void Renderer::BeginFrame(int width, int height)
    {
        glViewport(0, 0, width, height);
        glEnable(GL_DEPTH_TEST);
    }
    void Renderer::EndFrame() {}

    void Renderer::SetClearColor(float r, float g, float b, float a)
    {
        glClearColor(r, g, b, a);
    }

    void Renderer::Clear()
    {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    }

    void Renderer::DrawCube(float angleDegrees) const
    {
        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();

        GLint viewport[4]{};
        glGetIntegerv(GL_VIEWPORT, viewport);
        const int width = viewport[2] > 0 ? viewport[2] : 1;
        const int height = viewport[3] > 0 ? viewport[3] : 1;

        const float aspect = static_cast<float>(width) / static_cast<float>(height);
        const float zNear = 0.1f;
        const float zFar = 100.0f;
        const float fovYRadians = 60.0f * 3.14159265f / 180.0f;
        const float top = std::tan(fovYRadians * 0.5f) * zNear;
        const float right = top * aspect;
        glFrustum(-right, right, -top, top, zNear, zFar);

        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();
        glTranslatef(0.0f, 0.0f, -3.0f);
        glRotatef(angleDegrees, 0.6f, 1.0f, 0.2f);

        glBegin(GL_QUADS);
        glColor3f(1.0f, 0.2f, 0.2f);
        glVertex3f(-0.5f, -0.5f, 0.5f);
        glVertex3f(0.5f, -0.5f, 0.5f);
        glVertex3f(0.5f, 0.5f, 0.5f);
        glVertex3f(-0.5f, 0.5f, 0.5f);

        glColor3f(0.2f, 1.0f, 0.2f);
        glVertex3f(-0.5f, -0.5f, -0.5f);
        glVertex3f(-0.5f, 0.5f, -0.5f);
        glVertex3f(0.5f, 0.5f, -0.5f);
        glVertex3f(0.5f, -0.5f, -0.5f);

        glColor3f(0.2f, 0.2f, 1.0f);
        glVertex3f(-0.5f, -0.5f, -0.5f);
        glVertex3f(-0.5f, -0.5f, 0.5f);
        glVertex3f(-0.5f, 0.5f, 0.5f);
        glVertex3f(-0.5f, 0.5f, -0.5f);

        glColor3f(1.0f, 1.0f, 0.2f);
        glVertex3f(0.5f, -0.5f, -0.5f);
        glVertex3f(0.5f, 0.5f, -0.5f);
        glVertex3f(0.5f, 0.5f, 0.5f);
        glVertex3f(0.5f, -0.5f, 0.5f);

        glColor3f(0.2f, 1.0f, 1.0f);
        glVertex3f(-0.5f, 0.5f, -0.5f);
        glVertex3f(-0.5f, 0.5f, 0.5f);
        glVertex3f(0.5f, 0.5f, 0.5f);
        glVertex3f(0.5f, 0.5f, -0.5f);

        glColor3f(1.0f, 0.2f, 1.0f);
        glVertex3f(-0.5f, -0.5f, -0.5f);
        glVertex3f(0.5f, -0.5f, -0.5f);
        glVertex3f(0.5f, -0.5f, 0.5f);
        glVertex3f(-0.5f, -0.5f, 0.5f);
        glEnd();
    }

    void Renderer::DrawPyramid(float angleDegrees) const
    {
        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();

        GLint viewport[4]{};
        glGetIntegerv(GL_VIEWPORT, viewport);
        const int width = viewport[2] > 0 ? viewport[2] : 1;
        const int height = viewport[3] > 0 ? viewport[3] : 1;

        const float aspect = static_cast<float>(width) / static_cast<float>(height);
        const float zNear = 0.1f;
        const float zFar = 100.0f;
        const float fovYRadians = 60.0f * 3.14159265f / 180.0f;
        const float top = std::tan(fovYRadians * 0.5f) * zNear;
        const float right = top * aspect;
        glFrustum(-right, right, -top, top, zNear, zFar);

        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();
        glTranslatef(0.0f, -0.1f, -3.0f);
        glRotatef(angleDegrees, 0.2f, 1.0f, 0.0f);

        const float half = 0.7f;
        const float baseY = -0.6f;
        const float apexY = 0.6f;

        glBegin(GL_TRIANGLES);
        glColor3f(1.0f, 0.4f, 0.2f);
        glVertex3f(0.0f, apexY, 0.0f);
        glVertex3f(-half, baseY, half);
        glVertex3f(half, baseY, half);

        glColor3f(0.2f, 1.0f, 0.4f);
        glVertex3f(0.0f, apexY, 0.0f);
        glVertex3f(half, baseY, half);
        glVertex3f(half, baseY, -half);

        glColor3f(0.2f, 0.6f, 1.0f);
        glVertex3f(0.0f, apexY, 0.0f);
        glVertex3f(half, baseY, -half);
        glVertex3f(-half, baseY, -half);

        glColor3f(1.0f, 0.2f, 1.0f);
        glVertex3f(0.0f, apexY, 0.0f);
        glVertex3f(-half, baseY, -half);
        glVertex3f(-half, baseY, half);
        glEnd();

        glBegin(GL_QUADS);
        glColor3f(0.9f, 0.9f, 0.2f);
        glVertex3f(-half, baseY, half);
        glVertex3f(-half, baseY, -half);
        glVertex3f(half, baseY, -half);
        glVertex3f(half, baseY, half);
        glEnd();
    }
}
