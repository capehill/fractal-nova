#pragma once

#include "Vertex.hpp"
#include "EFractal.hpp"
#include "EPalette.hpp"

#include <proto/intuition.h>

namespace fractalnova {

enum class EFlag
{
    Refresh = 1,
    Resize = 2,
    Reset = 4
};

class GuiWindow
{
public:
    GuiWindow();
    ~GuiWindow();

    bool Run();

    void SetTitle(const char* title);

    Vertex GetPosition() const;
    void ClearPosition();
    float GetZoom() const;

    uint32 Width() const;
    uint32 Height() const;

    EFractal GetFractal() const;
    EPalette GetPalette() const;

    bool Flagged(EFlag flag) const;
    void Set(EFlag flag);
    void Clear(EFlag flag);

    Window* WindowPtr() const;

private:
    void HandleExtendedMouse(struct IntuiWheelData* data);
    bool HandleMenuPick();
    void HandleMouseButtons(UWORD code);
    void HandleMouseMove(int mouseX, int mouseY);
    void HandleNewSize();
    bool HandleRawKey(UWORD code);

    void ResetView();

    float GetZoomStep() const;
    void ZoomIn();
    void ZoomOut();

    Window* window { nullptr };

    bool panning { false };
    bool fastZoom { false };

    Vertex position { };
    float zoom { 1.0f };
    uint32 width { 800 };
    uint32 height { 600 };

    EFractal fractal { EFractal::Mandelbrot };
    EPalette palette { EPalette::Rainbow };

    uint32 flags { 0 };
};

} // fractalnova
