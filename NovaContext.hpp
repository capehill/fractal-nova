#pragma once

#include "Vertex.hpp"
#include "Palette.hpp" // Texture

#include <Warp3DNova/Context.h>

#include <string>
#include <memory>
#include <vector> // Texture

namespace fractalnova {

struct GuiWindow;

struct NovaObject
{
    NovaObject(W3DN_Context* context);

    std::string ErrorToString(W3DN_ErrorCode errCode) const;
    void ThrowOnError(W3DN_ErrorCode errCode, const std::string& message) const;

    W3DN_Context* context { nullptr };
};

struct DataBuffer: public NovaObject
{
    DataBuffer(W3DN_Context* context, W3DN_ShaderType shaderType, std::size_t size, W3DN_Shader* shader);
    ~DataBuffer();

    W3DN_ShaderType shaderType { W3DNST_END };
    W3DN_DataBuffer* dbo { nullptr };
};

struct Shader: public NovaObject
{
    Shader(W3DN_Context* context, W3DN_ShaderType shaderType, bool verbose);
    ~Shader();

    void Compile(const std::string& fileName);

    std::unique_ptr<DataBuffer> dbo;

    W3DN_Shader* shader { nullptr };
    bool verbose { false };
};

struct VertexBuffer: public NovaObject
{
    VertexBuffer(W3DN_Context* context);
    ~VertexBuffer();

    W3DN_VertexBuffer* vbo { nullptr };
};

struct Program: public NovaObject
{
    Program(W3DN_Context* context, bool verbose, int iterations);
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

struct Texture: public NovaObject
{
    Texture(W3DN_Context* context, const std::vector<Color>& colors);
    ~Texture();

    W3DN_Texture* texture { nullptr };
    W3DN_TextureSampler* sampler { nullptr };
};

struct BackBuffer
{
    BackBuffer(uint32 width, uint32 height, BitMap* friendBitMap);
    ~BackBuffer();

    BitMap* bitMap { nullptr };
};

struct NovaContext: public NovaObject
{
    NovaContext(const GuiWindow& window, bool verboseMode, bool vsync, int iterations);
    ~NovaContext();

    void CloseLib();

    void CreateTexture();

    void Resize();
    void Clear() const;
    void Draw() const;
    void SwapBuffers();

    void SetPosition(const Vertex& position);
    void SetZoom(float zoom);
    void Reset();

    std::unique_ptr<BackBuffer> backBuffer;
    std::unique_ptr<Program> program;
    std::unique_ptr<Texture> texture;

    const GuiWindow& window;
    uint32 width { 0 };
    uint32 height { 0 };
    bool verbose { false };
    bool vsync { false };
};

} // fractalnova
