#pragma once

#include <Warp3DNova/Context.h>

#include <string>

namespace fractalnova {

struct NovaObject
{
    explicit NovaObject(W3DN_Context* context);

    std::string ErrorToString(W3DN_ErrorCode errCode) const;
    void ThrowOnError(W3DN_ErrorCode errCode, const std::string& message) const;

    W3DN_Context* context { nullptr };
};

} // fractalnova

