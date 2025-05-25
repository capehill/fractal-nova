/*
Copyright (C) 2020-2025 Juha Niemimaki

This software is provided 'as-is', without any express or implied
warranty. In no event will the authors be held liable for any damages
arising from the use of this software.

Permission is granted to anyone to use this software for any purpose,
including commercial applications, and to alter it and redistribute it
freely, subject to the following restrictions:

1. The origin of this software must not be misrepresented; you must not
   claim that you wrote the original software. If you use this software
   in a product, an acknowledgment in the product documentation would be
   appreciated but is not required.
2. Altered source versions must be plainly marked as such, and must not be
   misrepresented as being the original software.
3. This notice may not be removed or altered from any source distribution.
*/

#pragma once

#include "Vertex.hpp"
#include "EFractal.hpp"
#include "EPalette.hpp"
#include "Logger.hpp"

#include <proto/intuition.h>

#include <bitset>

namespace fractalnova {

enum class EFlag
{
    Resize,
    Reset,
    ToggleFullscreen,
    Last
};

class BackBuffer;
enum class EMenu;

struct Resolution
{
    uint32 width { 640 };
    uint32 height { 480 };
};

struct Params
{
    bool vsync { false };
    bool fullscreen { false };
    bool lazyClear { false };
    int iterations { 100 };

    Resolution windowSize {};
    Resolution screenSize {};
};

class GuiWindow
{
public:
    GuiWindow(const Params& params);
    ~GuiWindow();

    bool Run();
    void Draw(const BackBuffer* backBuffer) const;

    void SetTitle(const char* title);

    Vertex GetPosition() const { return position; }
    void ClearPosition();

    float GetZoom() const { return zoom; }
    int GetIterations() const { return iterations; }

    uint32 Width() const { return windowSize.width; }
    uint32 Height() const { return windowSize.height; }

    EFractal GetFractal() const { return fractal; }
    EPalette GetPalette() const { return palette; }

    bool Flagged(EFlag flag) const;
    void Set(EFlag flag);
    void Clear(EFlag flag);

    Window* WindowPtr() const { return window; }

private:
    void CreateScreen();
    void CreateWindow();
    void DestroyWindow();
    Object* CreateMenu();
    void SetLimits() const;

    void HandleExtendedMouse(const struct IntuiWheelData* data);
    bool HandleMenuPick();
    void HandleMouseButtons(UWORD code);
    void HandleMouseMove(int mouseX, int mouseY);
    void HandleNewSize();
    bool HandleRawKey();
    void HandleIconify();
    void HandleUniconify();

    void ShowAboutWindow();

    void ResetView();

    float GetZoomStep() const;
    void ZoomIn();
    void ZoomOut();

    void ToggleMenuItem(const EMenu id, bool state);
    void ToggleVSync();
    void ToggleLogLevel(const EMenu id);
    void ToggleFullscreen();

    static uint32 IdcmpHook(Hook* hook, APTR window, IntuiMessage* msg);

    Object* windowObject { nullptr };
    Object* menuObject { nullptr };
    Window* window { nullptr };
    MsgPort* appPort { nullptr };
    Screen* screen { nullptr };

    bool panning { false };
    bool fastZoom { false };
    bool vsync { false };
    bool fullscreen { false };

    Vertex position { };
    float zoom { 1.0f };

    Resolution screenSize {};
    Resolution windowSize {};
    Resolution oldWindowSize {};

    EFractal fractal { EFractal::Mandelbrot };
    EPalette palette { EPalette::Rainbow };

    std::bitset<static_cast<unsigned>(EFlag::Last)> flags;
    int iterations { 100 };
};

} // fractalnova
