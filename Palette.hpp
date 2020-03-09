#pragma once

#include <cstdint>
#include <vector>

namespace fractalnova {

struct Color {
    uint8_t r;
    uint8_t g;
    uint8_t b;
    uint8_t a;

    Color(uint8_t r, uint8_t g, uint8_t b, uint8_t a = 255):
        r(r), g(g), b(b), a(a) {};
};

struct WeightedColor {
    Color c;
    float w;
};

struct Palette
{
    explicit Palette(int size);

    void Add(const Color& c, float w);
    std::vector<Color> Create();

    std::vector<WeightedColor> weightedColors;
    std::vector<Color> colors;
    const int size;
};

} // fractalnova

