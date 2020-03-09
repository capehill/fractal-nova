#include "Shader.hpp"
#include "DataBuffer.hpp"
#include "Logger.hpp"

namespace fractalnova {

namespace {
static constexpr bool verbose { false };
}

Shader::Shader(W3DN_Context* context, W3DN_ShaderType shaderType): NovaObject(context)
{
    if (shaderType == W3DNST_VERTEX) {
        dbo = std::make_unique<DataBuffer>(context, W3DNST_VERTEX, sizeof(VertexShaderData), shader);
    } else if (shaderType == W3DNST_FRAGMENT) {
        dbo = std::make_unique<DataBuffer>(context, W3DNST_FRAGMENT, sizeof(FragmentShaderData), shader);
    } else {
        throw std::runtime_error("Unknown shader type");
    }
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
        logging::Log("%s compilation log:\n%s", fileName.c_str(), shaderLog);
        context->DestroyShaderLog(shaderLog);
    }
}

} // fractalnova
