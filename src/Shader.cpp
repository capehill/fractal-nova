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

#include "Shader.hpp"
#include "DataBuffer.hpp"
#include "Logger.hpp"

namespace fractalnova {

namespace {
static constexpr bool verbose { false };
}

Shader::Shader(W3DN_Context* context, const std::string& fileName): NovaObject(context)
{
    logging::Debug("Create Shader %s", fileName.c_str());
    Compile(fileName);
}

Shader::~Shader()
{
    if (shader) {
        context->DestroyShader(shader);
        shader = nullptr;
    }

    dbo.reset();
}

void Shader::Compile(const std::string& fileName)
{
    W3DN_ErrorCode errCode;
    const char* shaderLog = nullptr;
    const std::string shaderPath = "shaders/" + fileName;

    shader = context->CompileShaderTags(&errCode,
        W3DNTag_FileName, shaderPath.c_str(),
        W3DNTag_Log, &shaderLog,
        W3DNTag_LogLevel, verbose ? W3DNLL_DEBUG : W3DNLL_INFO,
        TAG_DONE);

    if (!shader) {
        if (shaderLog) {
            logging::Error("Compiling %s failed (%u) with error: %s",
               fileName.c_str(), errCode, ErrorToString(errCode).c_str());

            context->DestroyShaderLog(shaderLog);
        }

        ThrowOnError(errCode, "Failed to compile shader " + fileName);
    }

    if (shaderLog) {
        logging::Detail("%s compilation log:\n%s", fileName.c_str(), shaderLog);
        context->DestroyShaderLog(shaderLog);
    }
}

DataBuffer* Shader::DboPtr() const
{
    return dbo.get();
}

W3DN_Shader* Shader::Ptr() const
{
    return shader;
}

} // fractalnova
