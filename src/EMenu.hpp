/*
Copyright (C) 2020-2025 Juha Niemimäki

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
    ToggleFullscreen,
    LogDetail,
    LogDebug,
    LogInfo,
    LogWarning,
    LogError,
    Iterations100,
    Iterations200,
    Iterations300,
    Iterations400,
    Iterations500,
    Iterations600,
    Iterations700,
    Iterations800,
    Iterations900,
    Iterations1000,
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

} // fractalnova
