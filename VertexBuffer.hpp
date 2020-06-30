#pragma once

#include "NovaObject.hpp"

namespace fractalnova {

class VertexBuffer: public NovaObject
{
public:
    explicit VertexBuffer(W3DN_Context* context);
    ~VertexBuffer();

    static constexpr uint32 vertexCount { 4 };

private:
    W3DN_VertexBuffer* vbo { nullptr };
};

} // fractalnova

