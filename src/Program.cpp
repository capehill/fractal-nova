/*
Copyright (C) 2020-2025 Juha Niemimaki

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

#include "Program.hpp"
#include "VertexShader.hpp"
#include "FragmentShader.hpp"
#include "Logger.hpp"

namespace fractalnova {

Program::Program(W3DN_Context* context, const int iterations, const char* name):
    NovaObject(context),
    vertexShader(std::make_unique<VertexShader>(context, name)),
    fragmentShader(std::make_unique<FragmentShader>(context, name)),
    iterations(iterations)
{
    logging::Debug("Creating shader program '%s', iterations %d", name, iterations);

    W3DN_ErrorCode errCode;
    shaderPipeline = context->CreateShaderPipelineTags(&errCode,
        W3DNTag_Shader, vertexShader->Ptr(),
        W3DNTag_Shader, fragmentShader->Ptr(),
        TAG_DONE);

    if (!shaderPipeline) {
        ThrowOnError(errCode, "Failed to create shader pipeline");
    }

    errCode = context->SetShaderPipeline(defaultRSO, shaderPipeline);

    ThrowOnError(errCode, "Failed to set shader pipeline");
}

Program::~Program()
{
    if (shaderPipeline) {
        context->SetShaderPipeline(defaultRSO, nullptr);
        context->DestroyShaderPipeline(shaderPipeline);
        shaderPipeline = nullptr;
    }

    vertexShader.reset();
    fragmentShader.reset();
}

void Program::UpdateVertexDBO() const
{
    oldPosition = vertexShader->UpdateDBO(zoom, oldPosition, position);
}

void Program::UpdateFragmentDBO() const
{
    fragmentShader->UpdateDBO(iterations, complex);
}

void Program::SetPosition(const Vertex& pos)
{
    position = pos;
}

void Program::SetComplex(const Vertex& c)
{
    complex = c;
}

void Program::SetZoom(const float z)
{
    zoom = z;
}

void Program::SetIterations(const int iter)
{
    iterations = iter;
}

void Program::Reset()
{
    oldPosition = { 0.0f, 0.0f };
}

} // fractalnova
