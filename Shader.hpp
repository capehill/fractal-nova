#pragma once

#include "NovaObject.hpp"
#include "Vertex.hpp"

#include <string>
#include <memory>

namespace fractalnova {

class DataBuffer;

struct VertexShaderData {
    float angle;
    float zoom;
    //Vertex zoom64;
    Vertex point;
};

struct FragmentShaderData {
    int32 iterations;
    Vertex complex;
};

class Shader: public NovaObject
{
public:
    Shader(W3DN_Context* context, W3DN_ShaderType shaderType);
    ~Shader();

    void Compile(const std::string& fileName);
    DataBuffer* DboPtr() const;
    W3DN_Shader* Ptr() const;

private:
    std::unique_ptr<DataBuffer> dbo;

    W3DN_Shader* shader { nullptr };
};

} // fractalnova

