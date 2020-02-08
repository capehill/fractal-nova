#pragma once

#include "Vertex.hpp"

#include <proto/intuition.h>

namespace fractalnova {

struct GuiWindow
{
    GuiWindow();
    ~GuiWindow();

    bool Run();
    void SetTitle(const char* title);

    Vertex GetPosition() const;
    void ClearPosition();
    float GetZoom() const;

    void ZoomIn();
    void ZoomOut();

    struct Window* window { nullptr };

    uint32 Width() const;
    uint32 Height() const;

    mutable bool resize { false };
    mutable bool refresh { true };
    mutable bool reset { false };

    bool panning { false };
    bool fastZoom { false };

    Vertex position { };
    float zoom { 1.0f };
    uint32 width { 800 };
    uint32 height { 600 };
};

} // fractalnova
