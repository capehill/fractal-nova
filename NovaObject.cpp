#include "NovaObject.hpp"

#include <proto/Warp3DNova.h>

#include <stdexcept>

namespace fractalnova {

extern struct Warp3DNovaIFace* IW3DNova;

NovaObject::NovaObject(W3DN_Context* context): context(context)
{
}

std::string NovaObject::ErrorToString(const W3DN_ErrorCode errCode) const
{
    return IW3DNova->W3DN_GetErrorString(errCode);
}

void NovaObject::ThrowOnError(const W3DN_ErrorCode errCode, const std::string& message) const
{
    if (errCode != W3DNEC_SUCCESS) {
        throw std::runtime_error(message + ": " + ErrorToString(errCode));
    }
}

} // fractalnova

