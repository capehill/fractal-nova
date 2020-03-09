#pragma once

#include "NovaObject.hpp"
#include "Vertex.hpp"

#include <string>
#include <memory>

namespace fractalnova {

struct DataBuffer;

struct VertexShaderData {
    float angle;
    float zoom;
    Vertex point;
};

struct FragmentShaderData {
    int32 iterations;
};

struct Shader: public NovaObject
{
    Shader(W3DN_Context* context, W3DN_ShaderType shaderType);
    ~Shader();

    void Compile(const std::string& fileName);

    std::unique_ptr<DataBuffer> dbo;

    W3DN_Shader* shader { nullptr };
};

} // fractalnova

