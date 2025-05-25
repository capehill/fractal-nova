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

#include "EPalette.hpp"

#include <cstdint>
#include <vector>

namespace fractalnova {

struct Color
{
    Color(uint8_t r, uint8_t g, uint8_t b, uint8_t a = 255):
        r(r), g(g), b(b), a(a) {};

    uint8_t r;
    uint8_t g;
    uint8_t b;
    uint8_t a;
};

struct WeightedColor {
    Color c;
    float w;
};

class Palette
{
public:
    explicit Palette(EPalette palette, int size = 4 * 256);

    void Create(EPalette palette);

    std::vector<Color> GetColorArray();

private:
    void Rainbow();
    void RainbowRev();
    void Red();
    void Green();
    void Blue();
    void BlackAndWhite();
    void BlackAndWhiteRev();

    void Add(const Color& c, float w);

    std::vector<WeightedColor> weightedColors;
    std::vector<Color> colors;
    const int size;
};

} // fractalnova
