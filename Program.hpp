#pragma once

#include "NovaObject.hpp"
#include "Vertex.hpp"

#include <memory>

namespace fractalnova {

struct VertexBuffer;
struct Shader;

struct Program: public NovaObject
{
    Program(W3DN_Context* context, int iterations, const char* name);
    ~Program();

    void SetPosition(const Vertex& pos);
    void SetZoom(float z);
    void Reset();

    void UpdateVertexDBO() const;
    void UpdateFragmentDBO() const;

    std::unique_ptr<VertexBuffer> vbo;

    std::unique_ptr<Shader> vertexShader;
    std::unique_ptr<Shader> fragmentShader;

    W3DN_ShaderPipeline* shaderPipeline { nullptr };

    float zoom { 1.0f };
    Vertex position { };
    mutable Vertex oldPosition { };

    int iterations { 0 };
};

} // fractalnova

