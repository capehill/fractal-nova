#include "VertexBuffer.hpp"
#include "Vertex.hpp"

namespace fractalnova {

static constexpr uint32 posAttributeIndex { 0 };
static constexpr uint32 texCoordAttributeIndex { 1 };

VertexBuffer::~VertexBuffer()
{
    if (vbo) {
        constexpr uint32 arrayIdx = 0;

        context->BindVertexAttribArray(defaultRSO, posAttributeIndex, nullptr, arrayIdx);
        context->BindVertexAttribArray(defaultRSO, texCoordAttributeIndex, nullptr, arrayIdx);
        context->DestroyVertexBufferObject(vbo);
        vbo = nullptr;
    }
}

VertexBuffer::VertexBuffer(W3DN_Context* context): NovaObject(context)
{
    constexpr uint32 arrayCount { 2 };

    W3DN_ErrorCode errCode;

    vbo = context->CreateVertexBufferObjectTags(&errCode,
        vertexCount * sizeof(Vertex4), W3DN_STATIC_DRAW, arrayCount, TAG_DONE);

    ThrowOnError(errCode, "Failed create vertex buffer object");

    constexpr uint32 posArrayIndex { 0 };
    constexpr uint32 texCoordArrayIndex { 1 };

    constexpr uint32 stride { sizeof(Vertex4) };
    constexpr uint32 posElementCount { 2 };
    constexpr uint32 texCoordElementCount { 2 };
    constexpr uint32 posBase { 0 };
    constexpr uint32 texCoordBase { posElementCount * sizeof(float) };

    errCode = context->VBOSetArray(vbo, posArrayIndex, W3DNEF_FLOAT, FALSE, posElementCount, stride, posBase, vertexCount);

    ThrowOnError(errCode, "Failed to set VBO array (pos)");

    errCode = context->VBOSetArray(vbo, texCoordArrayIndex, W3DNEF_FLOAT, FALSE, texCoordElementCount, stride, texCoordBase, vertexCount);

    ThrowOnError(errCode, "Failed to set VBO array (texCoord)");

    constexpr uint64 readOffset = 0;
    constexpr uint64 readSize = 0;

    W3DN_BufferLock* lock = context->VBOLock(&errCode, vbo, readOffset, readSize);

    ThrowOnError(errCode, "Failed to lock vertex buffer object");

    Vertex4* vertices = reinterpret_cast<Vertex4 *>(lock->buffer);

    // Make a quad
    //
    // 2---3
    // | \ |
    // 0---1

    vertices[0].x = -1.0f;
    vertices[0].y = -1.0f;
    vertices[0].s = -1.0f;
    vertices[0].t = -1.0f;

    vertices[1].x =  1.0f;
    vertices[1].y = -1.0f;
    vertices[1].s =  1.0f;
    vertices[1].t = -1.0f;

    vertices[2].x = -1.0f;
    vertices[2].y =  1.0f;
    vertices[2].s = -1.0f;
    vertices[2].t =  1.0f;

    vertices[3].x = 1.0f;
    vertices[3].y = 1.0f;
    vertices[3].s = 1.0f;
    vertices[3].t = 1.0f;

    constexpr uint64 writeOffset = 0;

    errCode = context->BufferUnlock(lock, writeOffset, lock->size);

    ThrowOnError(errCode, "Failed to unlock vertex buffer object");

    errCode = context->BindVertexAttribArray(defaultRSO, posAttributeIndex, vbo, posArrayIndex);

    ThrowOnError(errCode, "Failed to bind vertex attribute array (pos)");

    errCode = context->BindVertexAttribArray(defaultRSO, texCoordAttributeIndex, vbo, texCoordArrayIndex);

    ThrowOnError(errCode, "Failed to bind vertex attribute array (texCoord)");
}

} // fractalnova
