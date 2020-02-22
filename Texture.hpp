#pragma once

#include "NovaObject.hpp"
#include "Palette.hpp"

#include <vector>

namespace fractalnova {

struct Texture: public NovaObject
{
    Texture(W3DN_Context* context, const std::vector<Color>& colors);
    ~Texture();

    W3DN_Texture* texture { nullptr };
    W3DN_TextureSampler* sampler { nullptr };
};

} // fractalnova

