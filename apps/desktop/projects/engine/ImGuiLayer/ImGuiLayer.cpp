#include "Engine/ImGuiLayer/ImGuiLayer.hpp"

#include <imgui/imgui.h>

#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <Windows.h>
#include <Windowsx.h>
#ifdef CreateWindow
#undef CreateWindow
#endif
#endif

#include <GL/gl.h>

#include <algorithm>

namespace Engine::ImGuiLayer
{
    namespace
    {
        bool g_initialized = false;
        GLuint g_fontTexture = 0;

        void CreateFontsTexture()
        {
            ImGuiIO& io = ImGui::GetIO();

            unsigned char* pixels = nullptr;
            int width = 0;
            int height = 0;
            io.Fonts->GetTexDataAsRGBA32(&pixels, &width, &height);

            glGenTextures(1, &g_fontTexture);
            glBindTexture(GL_TEXTURE_2D, g_fontTexture);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, pixels);

            io.Fonts->SetTexID(static_cast<ImTextureID>(g_fontTexture));
        }

        void DestroyFontsTexture()
        {
            if (g_fontTexture != 0)
            {
                glDeleteTextures(1, &g_fontTexture);
                g_fontTexture = 0;
                ImGui::GetIO().Fonts->SetTexID(0);
            }
        }

        void SetupRenderState(const ImDrawData* drawData, int fbWidth, int fbHeight)
        {
            glEnable(GL_BLEND);
            glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
            glDisable(GL_CULL_FACE);
            glDisable(GL_DEPTH_TEST);
            glEnable(GL_SCISSOR_TEST);
            glEnable(GL_TEXTURE_2D);

            glViewport(0, 0, fbWidth, fbHeight);

            glMatrixMode(GL_PROJECTION);
            glPushMatrix();
            glLoadIdentity();
            glOrtho(drawData->DisplayPos.x, drawData->DisplayPos.x + drawData->DisplaySize.x,
                    drawData->DisplayPos.y + drawData->DisplaySize.y, drawData->DisplayPos.y,
                    -1.0f, 1.0f);
            glMatrixMode(GL_MODELVIEW);
            glPushMatrix();
            glLoadIdentity();

            glMatrixMode(GL_TEXTURE);
            glPushMatrix();
            glLoadIdentity();

            glEnableClientState(GL_VERTEX_ARRAY);
            glEnableClientState(GL_TEXTURE_COORD_ARRAY);
            glEnableClientState(GL_COLOR_ARRAY);
        }

        void RestoreRenderState()
        {
            glDisableClientState(GL_VERTEX_ARRAY);
            glDisableClientState(GL_TEXTURE_COORD_ARRAY);
            glDisableClientState(GL_COLOR_ARRAY);

            glMatrixMode(GL_TEXTURE);
            glPopMatrix();
            glMatrixMode(GL_MODELVIEW);
            glPopMatrix();
            glMatrixMode(GL_PROJECTION);
            glPopMatrix();

            glDisable(GL_SCISSOR_TEST);
            glDisable(GL_BLEND);
        }

        void RenderDrawData(ImDrawData* drawData)
        {
            const int fbWidth = static_cast<int>(drawData->DisplaySize.x * drawData->FramebufferScale.x);
            const int fbHeight = static_cast<int>(drawData->DisplaySize.y * drawData->FramebufferScale.y);
            if (fbWidth <= 0 || fbHeight <= 0)
            {
                return;
            }

            SetupRenderState(drawData, fbWidth, fbHeight);

            const ImVec2 clipOff = drawData->DisplayPos;
            const ImVec2 clipScale = drawData->FramebufferScale;

            for (int n = 0; n < drawData->CmdListsCount; n++)
            {
                const ImDrawList* cmdList = drawData->CmdLists[n];
                const ImDrawVert* vtxBuffer = cmdList->VtxBuffer.Data;
                const ImDrawIdx* idxBuffer = cmdList->IdxBuffer.Data;

                glVertexPointer(2, GL_FLOAT, sizeof(ImDrawVert), reinterpret_cast<const void*>(&vtxBuffer->pos));
                glTexCoordPointer(2, GL_FLOAT, sizeof(ImDrawVert), reinterpret_cast<const void*>(&vtxBuffer->uv));
                glColorPointer(4, GL_UNSIGNED_BYTE, sizeof(ImDrawVert), reinterpret_cast<const void*>(&vtxBuffer->col));

                for (int cmd_i = 0; cmd_i < cmdList->CmdBuffer.Size; cmd_i++)
                {
                    const ImDrawCmd* pcmd = &cmdList->CmdBuffer[cmd_i];
                    if (pcmd->UserCallback != nullptr)
                    {
                        pcmd->UserCallback(cmdList, pcmd);
                    }
                    else
                    {
                        ImVec4 clipRect;
                        clipRect.x = (pcmd->ClipRect.x - clipOff.x) * clipScale.x;
                        clipRect.y = (pcmd->ClipRect.y - clipOff.y) * clipScale.y;
                        clipRect.z = (pcmd->ClipRect.z - clipOff.x) * clipScale.x;
                        clipRect.w = (pcmd->ClipRect.w - clipOff.y) * clipScale.y;

                        if (clipRect.x < fbWidth && clipRect.y < fbHeight && clipRect.z >= 0.0f && clipRect.w >= 0.0f)
                        {
                            glScissor(
                                static_cast<int>(clipRect.x),
                                static_cast<int>(fbHeight - clipRect.w),
                                static_cast<int>(clipRect.z - clipRect.x),
                                static_cast<int>(clipRect.w - clipRect.y));

                            const GLuint texture = static_cast<GLuint>(pcmd->GetTexID());
                            glBindTexture(GL_TEXTURE_2D, texture);

#if defined(IMGUI_USE_BGRA_PACKED_COLOR)
                            glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(pcmd->ElemCount),
                                           sizeof(ImDrawIdx) == 2 ? GL_UNSIGNED_SHORT : GL_UNSIGNED_INT,
                                           idxBuffer);
#else
                            glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(pcmd->ElemCount),
                                           sizeof(ImDrawIdx) == 2 ? GL_UNSIGNED_SHORT : GL_UNSIGNED_INT,
                                           idxBuffer);
#endif
                        }
                    }

                    idxBuffer += pcmd->ElemCount;
                }
            }

            RestoreRenderState();
        }

#ifdef _WIN32
        ImGuiKey VirtualKeyToImGuiKey(WPARAM wParam)
        {
            switch (wParam)
            {
            case VK_TAB: return ImGuiKey_Tab;
            case VK_LEFT: return ImGuiKey_LeftArrow;
            case VK_RIGHT: return ImGuiKey_RightArrow;
            case VK_UP: return ImGuiKey_UpArrow;
            case VK_DOWN: return ImGuiKey_DownArrow;
            case VK_PRIOR: return ImGuiKey_PageUp;
            case VK_NEXT: return ImGuiKey_PageDown;
            case VK_HOME: return ImGuiKey_Home;
            case VK_END: return ImGuiKey_End;
            case VK_INSERT: return ImGuiKey_Insert;
            case VK_DELETE: return ImGuiKey_Delete;
            case VK_BACK: return ImGuiKey_Backspace;
            case VK_SPACE: return ImGuiKey_Space;
            case VK_RETURN: return ImGuiKey_Enter;
            case VK_ESCAPE: return ImGuiKey_Escape;
            case VK_OEM_7: return ImGuiKey_Apostrophe;
            case VK_OEM_COMMA: return ImGuiKey_Comma;
            case VK_OEM_MINUS: return ImGuiKey_Minus;
            case VK_OEM_PERIOD: return ImGuiKey_Period;
            case VK_OEM_2: return ImGuiKey_Slash;
            case VK_OEM_1: return ImGuiKey_Semicolon;
            case VK_OEM_PLUS: return ImGuiKey_Equal;
            case VK_OEM_4: return ImGuiKey_LeftBracket;
            case VK_OEM_5: return ImGuiKey_Backslash;
            case VK_OEM_6: return ImGuiKey_RightBracket;
            case VK_OEM_3: return ImGuiKey_GraveAccent;
            case VK_CAPITAL: return ImGuiKey_CapsLock;
            case VK_SCROLL: return ImGuiKey_ScrollLock;
            case VK_NUMLOCK: return ImGuiKey_NumLock;
            case VK_SNAPSHOT: return ImGuiKey_PrintScreen;
            case VK_PAUSE: return ImGuiKey_Pause;
            case VK_NUMPAD0: return ImGuiKey_Keypad0;
            case VK_NUMPAD1: return ImGuiKey_Keypad1;
            case VK_NUMPAD2: return ImGuiKey_Keypad2;
            case VK_NUMPAD3: return ImGuiKey_Keypad3;
            case VK_NUMPAD4: return ImGuiKey_Keypad4;
            case VK_NUMPAD5: return ImGuiKey_Keypad5;
            case VK_NUMPAD6: return ImGuiKey_Keypad6;
            case VK_NUMPAD7: return ImGuiKey_Keypad7;
            case VK_NUMPAD8: return ImGuiKey_Keypad8;
            case VK_NUMPAD9: return ImGuiKey_Keypad9;
            case VK_DECIMAL: return ImGuiKey_KeypadDecimal;
            case VK_DIVIDE: return ImGuiKey_KeypadDivide;
            case VK_MULTIPLY: return ImGuiKey_KeypadMultiply;
            case VK_SUBTRACT: return ImGuiKey_KeypadSubtract;
            case VK_ADD: return ImGuiKey_KeypadAdd;
            case VK_LSHIFT: return ImGuiKey_LeftShift;
            case VK_LCONTROL: return ImGuiKey_LeftCtrl;
            case VK_LMENU: return ImGuiKey_LeftAlt;
            case VK_LWIN: return ImGuiKey_LeftSuper;
            case VK_RSHIFT: return ImGuiKey_RightShift;
            case VK_RCONTROL: return ImGuiKey_RightCtrl;
            case VK_RMENU: return ImGuiKey_RightAlt;
            case VK_RWIN: return ImGuiKey_RightSuper;
            case VK_APPS: return ImGuiKey_Menu;
            }

            if (wParam >= '0' && wParam <= '9')
            {
                return static_cast<ImGuiKey>(ImGuiKey_0 + (wParam - '0'));
            }
            if (wParam >= 'A' && wParam <= 'Z')
            {
                return static_cast<ImGuiKey>(ImGuiKey_A + (wParam - 'A'));
            }

            return ImGuiKey_None;
        }
#endif
    }

    void ImGuiLayer::Init(Platform::Window& window)
    {
        if (g_initialized)
        {
            return;
        }

        IMGUI_CHECKVERSION();
        ImGui::CreateContext();

        ImGuiIO& io = ImGui::GetIO();
        io.BackendPlatformName = "GAM300_Win32";
        io.BackendRendererName = "GAM300_OpenGL1";
        io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;

        io.DisplaySize = ImVec2(static_cast<float>(window.Width()), static_cast<float>(window.Height()));
        io.DeltaTime = 1.0f / 60.0f;

        CreateFontsTexture();

        g_initialized = true;
    }

    void ImGuiLayer::Shutdown()
    {
        if (!g_initialized)
        {
            return;
        }

        DestroyFontsTexture();
        ImGui::DestroyContext();
        g_initialized = false;
    }

    void ImGuiLayer::BeginFrame(Platform::Window& window, double deltaSeconds)
    {
        if (!g_initialized)
        {
            Init(window);
        }

        ImGuiIO& io = ImGui::GetIO();
        io.DisplaySize = ImVec2(static_cast<float>(window.Width()), static_cast<float>(window.Height()));
        io.DeltaTime = static_cast<float>(std::max(deltaSeconds, 1.0 / 1000.0));

        ImGui::NewFrame();
    }

    void ImGuiLayer::EndFrame()
    {
        if (!g_initialized)
        {
            return;
        }

        ImGui::Render();
        RenderDrawData(ImGui::GetDrawData());
    }

#ifdef _WIN32
    bool HandleWin32Message(void* hwnd, unsigned int msg, std::uintptr_t wParam, std::intptr_t lParam)
    {
        if (!ImGui::GetCurrentContext())
        {
            return false;
        }

        ImGuiIO& io = ImGui::GetIO();

        switch (msg)
        {
        case WM_LBUTTONDOWN:
            io.AddMouseButtonEvent(0, true);
            SetCapture(reinterpret_cast<HWND>(hwnd));
            return true;
        case WM_LBUTTONUP:
            io.AddMouseButtonEvent(0, false);
            ReleaseCapture();
            return true;
        case WM_RBUTTONDOWN:
            io.AddMouseButtonEvent(1, true);
            SetCapture(reinterpret_cast<HWND>(hwnd));
            return true;
        case WM_RBUTTONUP:
            io.AddMouseButtonEvent(1, false);
            ReleaseCapture();
            return true;
        case WM_MBUTTONDOWN:
            io.AddMouseButtonEvent(2, true);
            SetCapture(reinterpret_cast<HWND>(hwnd));
            return true;
        case WM_MBUTTONUP:
            io.AddMouseButtonEvent(2, false);
            ReleaseCapture();
            return true;
        case WM_MOUSEWHEEL:
            io.AddMouseWheelEvent(0.0f, static_cast<float>(GET_WHEEL_DELTA_WPARAM(wParam)) / static_cast<float>(WHEEL_DELTA));
            return true;
        case WM_MOUSEHWHEEL:
            io.AddMouseWheelEvent(static_cast<float>(GET_WHEEL_DELTA_WPARAM(wParam)) / static_cast<float>(WHEEL_DELTA), 0.0f);
            return true;
        case WM_MOUSEMOVE:
            io.AddMousePosEvent(static_cast<float>(GET_X_LPARAM(lParam)), static_cast<float>(GET_Y_LPARAM(lParam)));
            return true;
        case WM_KEYDOWN:
        case WM_SYSKEYDOWN:
        {
            const ImGuiKey key = VirtualKeyToImGuiKey(static_cast<WPARAM>(wParam));
            if (key != ImGuiKey_None)
            {
                io.AddKeyEvent(key, true);
            }
            io.AddKeyEvent(ImGuiKey_ModCtrl, (GetKeyState(VK_CONTROL) & 0x8000) != 0);
            io.AddKeyEvent(ImGuiKey_ModShift, (GetKeyState(VK_SHIFT) & 0x8000) != 0);
            io.AddKeyEvent(ImGuiKey_ModAlt, (GetKeyState(VK_MENU) & 0x8000) != 0);
            io.AddKeyEvent(ImGuiKey_ModSuper, (GetKeyState(VK_LWIN) & 0x8000) != 0 || (GetKeyState(VK_RWIN) & 0x8000) != 0);
            return true;
        }
        case WM_KEYUP:
        case WM_SYSKEYUP:
        {
            const ImGuiKey key = VirtualKeyToImGuiKey(static_cast<WPARAM>(wParam));
            if (key != ImGuiKey_None)
            {
                io.AddKeyEvent(key, false);
            }
            io.AddKeyEvent(ImGuiKey_ModCtrl, (GetKeyState(VK_CONTROL) & 0x8000) != 0);
            io.AddKeyEvent(ImGuiKey_ModShift, (GetKeyState(VK_SHIFT) & 0x8000) != 0);
            io.AddKeyEvent(ImGuiKey_ModAlt, (GetKeyState(VK_MENU) & 0x8000) != 0);
            io.AddKeyEvent(ImGuiKey_ModSuper, (GetKeyState(VK_LWIN) & 0x8000) != 0 || (GetKeyState(VK_RWIN) & 0x8000) != 0);
            return true;
        }
        case WM_CHAR:
            io.AddInputCharacter(static_cast<unsigned int>(wParam));
            return true;
        }

        return false;
    }
#endif
}
