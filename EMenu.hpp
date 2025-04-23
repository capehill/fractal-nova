#pragma once

#include <intuition/menuclass.h>

namespace fractalnova {

enum class EMenu {
    NoMenuId = NO_MENU_ID,
    Iconify = 1,
    About,
    Quit,
    // Control
    ResetView,
    VSync,
    LogDetail,
    LogDebug,
    LogInfo,
    LogWarning,
    LogError,
    // Fractals
    Mandelbrot,
    Julia1,
    Julia2,
    Julia3,
    Julia4,
    Julia5,
    Julia6,
    Julia7,
    Julia8,
    Julia9,
    Julia10,
    // Palettes
    Rainbow,
    RainbowRev,
    Red,
    Green,
    Blue,
    BlackAndWhite,
    BlackAndWhiteRev
};

} // factalnova
