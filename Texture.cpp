#include "Texture.hpp"

namespace fractalnova {

static constexpr bool textureFiltering { false };

Texture::Texture(W3DN_Context* context, const std::vector<Color>& colors): NovaObject(context)
{
    W3DN_ErrorCode errCode;

    texture = context->CreateTexture(&errCode, W3DN_TEXTURE_2D, W3DNPF_RGBA, W3DNEF_UINT8,
        colors.size(), 1, 1 /* depth */, FALSE /* mipmapped*/, W3DN_STATIC_DRAW);

    ThrowOnError(errCode, "Failed to create texture");

    errCode = context->TexUpdateImage(texture, const_cast<Color*>(colors.data()), 0 /* level */, 0 /* arrayIdx */, sizeof(Color) * colors.size(), 0 /* srcRowsPerLayer */);

    ThrowOnError(errCode, "Failed to update texture");

    sampler = context->CreateTexSampler(&errCode);

    ThrowOnError(errCode, "Failed to create texture sampler");

    errCode = context->TSSetParametersTags(sampler,
        W3DN_TEXTURE_MIN_FILTER, textureFiltering ? W3DN_LINEAR : W3DN_NEAREST,
        TAG_DONE);

    ThrowOnError(errCode, "Failed to set texture sampler parameters");

    context->BindTexture(nullptr, 0 /* texture unit */, texture, sampler);

    ThrowOnError(errCode, "Failed to bind texture");
}

Texture::~Texture()
{
    if (texture) {
        context->BindTexture(nullptr, 0, nullptr, nullptr);
        context->DestroyTexture(texture);
        texture = nullptr;
    }

    if (sampler) {
        context->DestroyTexSampler(sampler);
        sampler = nullptr;
    }
}

} // fractalnova
