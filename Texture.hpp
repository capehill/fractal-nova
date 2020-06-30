#pragma once

#include "NovaObject.hpp"
#include "Palette.hpp"

#include <vector>

namespace fractalnova {

class Texture: public NovaObject
{
public:
    Texture(W3DN_Context* context, const std::vector<Color>& colors);
    ~Texture();

private:
    W3DN_Texture* texture { nullptr };
    W3DN_TextureSampler* sampler { nullptr };
};

} // fractalnova

