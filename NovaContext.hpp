#pragma once

#include "Vertex.hpp"

#include <Warp3DNova/Context.h>

#include <string>

namespace fractalnova {

struct GuiWindow;

struct NovaContext
{
    NovaContext(const GuiWindow& window, bool verboseMode, bool vsync);
    ~NovaContext();

    void CloseLib();

    W3DN_Shader* CompileShader(const std::string& fileName);
    void LoadShaders();
    void CreateVBO();
    void CreateDBO();

    void Resize();
    void Clear() const;
    void Draw() const;
    void SwapBuffers();

    void SetPosition(const Vertex& position);
    void SetZoom(float zoom);

    BitMap* backBuffer { nullptr };
    W3DN_Context* context { nullptr };

    W3DN_Shader* vertexShader { nullptr };
    W3DN_Shader* fragmentShader { nullptr };

    W3DN_ShaderPipeline* shaderPipeline { nullptr };
    W3DN_VertexBuffer* vbo { nullptr };
    W3DN_DataBuffer* dbo { nullptr };

    std::string ErrorToString(W3DN_ErrorCode errCode) const;
    void ThrowOnError(W3DN_ErrorCode errCode, const std::string& message) const;

    const GuiWindow& window;
    uint32 width { 0 };
    uint32 height { 0 };
    bool verbose { false };
    bool vsync { false };

    float zoom { 1.0f };
    Vertex position { };
};

} // fractalnova
