/*
Copyright (C) 2020-2025 Juha Niemimäki

This software is provided 'as-is', without any express or implied
warranty. In no event will the authors be held liable for any damages
arising from the use of this software.

Permission is granted to anyone to use this software for any purpose,
including commercial applications, and to alter it and redistribute it
freely, subject to the following restrictions:

1. The origin of this software must not be misrepresented; you must not
   claim that you wrote the original software. If you use this software
   in a product, an acknowledgment in the product documentation would be
   appreciated but is not required.
2. Altered source versions must be plainly marked as such, and must not be
   misrepresented as being the original software.
3. This notice may not be removed or altered from any source distribution.
*/

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
    void SetIterations(int iterations);
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
