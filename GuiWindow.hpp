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
    float GetZoom() const;
    void ZoomIn();
    void ZoomOut();

    struct Window* window { nullptr };

    mutable bool resize { false };
    mutable bool refresh { true };

    Vertex position { };
    float zoom { 1.0f };
};

} // fractalnova
