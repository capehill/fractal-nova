#pragma once

#include "NovaObject.hpp"

namespace fractalnova {

struct VertexBuffer: public NovaObject
{
    VertexBuffer(W3DN_Context* context);
    ~VertexBuffer();

    W3DN_VertexBuffer* vbo { nullptr };

    static constexpr uint32 vertexCount { 4 };
};

} // fractalnova

