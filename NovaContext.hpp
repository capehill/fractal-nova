#pragma once

#include <proto/warp3dnova.h>

#include <string>

namespace fractalnova {

struct GuiWindow;

struct NovaContext
{
    NovaContext(const GuiWindow& window);
    ~NovaContext();

    void CloseLib();

    W3DN_Shader* CompileShader(const std::string& fileName);
    void LoadShaders();
    void CreateVBO();

    void Resize();
    void Clear() const;
    void Draw() const;
    void SwapBuffers();

    struct BitMap* backBuffer { nullptr };
    W3DN_Context* context { nullptr };

    W3DN_Shader* vertexShader { nullptr };
    W3DN_Shader* fragmentShader { nullptr };

    W3DN_ShaderPipeline* shaderPipeline { nullptr };
    W3DN_VertexBuffer* vbo { nullptr };

    std::string ErrorToString(W3DN_ErrorCode errCode) const;

    const GuiWindow& window;
    uint32 width { 0 };
    uint32 height { 0 };
};

} // fractalnova
