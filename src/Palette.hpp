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

