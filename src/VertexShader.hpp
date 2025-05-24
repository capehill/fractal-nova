#pragma once

#include "Shader.hpp"

namespace fractalnova {

struct Vertex;

class VertexShader: public Shader
{
public:
    VertexShader(W3DN_Context* context, const std::string& fileName);
    ~VertexShader() = default;

    Vertex UpdateDBO(float zoom, const Vertex& oldPosition, const Vertex& position) const;

private:
};

} // fractalnova


