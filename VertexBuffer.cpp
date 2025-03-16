#include "VertexBuffer.hpp"
#include "Vertex.hpp"

namespace fractalnova {

static constexpr uint32 posArrayIndex { 0 };
static constexpr uint32 texCoordArrayIndex { 1 };

VertexBuffer::~VertexBuffer()
{
    if (vbo) {
        context->BindVertexAttribArray(defaultRSO, 0 /* attribNum*/, nullptr, 0 /* arrayIdx */);
        context->BindVertexAttribArray(defaultRSO, 1 /* attribNum*/, nullptr, 0 /* arrayIdx */);
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

    constexpr uint32 stride { sizeof(Vertex4) };
    constexpr uint32 posElementCount { 2 };
    constexpr uint32 texCoordElementCount { 2 };

    errCode = context->VBOSetArray(vbo, posArrayIndex, W3DNEF_FLOAT, FALSE, posElementCount, stride, 0, vertexCount);

    ThrowOnError(errCode, "Failed to set VBO array");

    errCode = context->VBOSetArray(vbo, texCoordArrayIndex, W3DNEF_FLOAT, FALSE, texCoordElementCount, stride, 0 /* FIXME */, vertexCount);

    ThrowOnError(errCode, "Failed to set VBO array (texCoord)");

    W3DN_BufferLock* lock = context->VBOLock(&errCode, vbo, 0, 0);

    ThrowOnError(errCode, "Failed to lock vertex buffer object");

    Vertex4* vertices = reinterpret_cast<Vertex4 *>(lock->buffer);

    // Make a quad
    vertices[0].x = -1.0f;
    vertices[0].y = -1.0f;
    vertices[0].s = 0.0f;
    vertices[0].t = 0.0f;

    vertices[1].x = -1.0f;
    vertices[1].y =  1.0f;
    vertices[1].s =  0.0f;
    vertices[1].t =  1.0f;

    vertices[2].x =  1.0f;
    vertices[2].y = -1.0f;
    vertices[2].s =  1.0f;
    vertices[2].t =  1.0f;

    vertices[3].x = 1.0f;
    vertices[3].y = 1.0f;
    vertices[3].s = 1.0f;
    vertices[3].t = 0.0f;

    errCode = context->BufferUnlock(lock, 0, lock->size);

    ThrowOnError(errCode, "Failed to unlock vertex buffer object");

    constexpr uint32 posAttributeIndex { 0 };
    constexpr uint32 texCoordAttributeIndex { 0 };

    errCode = context->BindVertexAttribArray(defaultRSO, posArrayIndex, vbo, posAttributeIndex);

    ThrowOnError(errCode, "Failed to bind vertex attribute array (pos)");

    errCode = context->BindVertexAttribArray(defaultRSO, texCoordArrayIndex, vbo, texCoordAttributeIndex);

    ThrowOnError(errCode, "Failed to bind vertex attribute array (texCoord)");
}

} // fractalnova
