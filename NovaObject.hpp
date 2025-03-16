#pragma once

#include <Warp3DNova/Context.h>

#include <string>

namespace fractalnova {

class NovaObject
{
public:
    explicit NovaObject(W3DN_Context* context);

protected:
    std::string ErrorToString(W3DN_ErrorCode errCode) const;
    void ThrowOnError(W3DN_ErrorCode errCode, const std::string& message) const;

    W3DN_Context* context { nullptr };
    W3DN_RenderState* const defaultRSO { nullptr };
};

} // fractalnova

