#pragma once

#include "NovaObject.hpp"
#include "Vertex.hpp"

#include <memory>

namespace fractalnova {

class VertexShader;
class FragmentShader;

class Program: public NovaObject
{
public:
    Program(W3DN_Context* context, int iterations, const char* name);
    ~Program();

    void SetPosition(const Vertex& pos);
    void SetComplex(const Vertex& complex);
    void SetZoom(float z);
    void Reset();

    void UpdateVertexDBO() const;
    void UpdateFragmentDBO() const;

private:
    std::unique_ptr<VertexShader> vertexShader;
    std::unique_ptr<FragmentShader> fragmentShader;

    W3DN_ShaderPipeline* shaderPipeline { nullptr };

    float zoom { 1.0f };
    //double zoom64 { 1.0f };
    Vertex position { };
    Vertex complex { };
    mutable Vertex oldPosition { };

    int iterations { 0 };
};

} // fractalnova

