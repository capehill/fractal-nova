#pragma once

#include <proto/intuition.h>

namespace fractalnova {

struct GuiWindow
{
    GuiWindow();
    ~GuiWindow();

    bool Run();

    struct Window* window { nullptr };

    mutable bool resize { false };
    mutable bool refresh { true };
};

} // fractalnova
