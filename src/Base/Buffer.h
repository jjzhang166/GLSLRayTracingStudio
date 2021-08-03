#pragma once

#include "Common/Common.h"

#include <glad/glad.h>

class GLBuffer
{
public:

    GLBuffer(GLenum target, GLuint usage);

    virtual ~GLBuffer();

    void Active();

    void Deactive();

    void Upload(uint8* bytes, int32 length);

    FORCEINLINE int32 Length() const
    {
        return m_Length;
    }

    FORCEINLINE GLuint Object() const
    {
        return m_Buffer;
    }

    FORCEINLINE GLuint Usage() const
    {
        return m_Usage;
    }

    FORCEINLINE GLenum Target() const
    {
        return m_Target;
    }

protected:

    GLuint      m_Buffer;
    GLenum      m_Target;
    GLuint      m_Usage;
    int32       m_Length;
};

class IndexBuffer : public GLBuffer
{
public:

    IndexBuffer()
        : GLBuffer(GL_ELEMENT_ARRAY_BUFFER, GL_STATIC_DRAW)
    {

    }

    virtual ~IndexBuffer()
    {

    }
};

class VertexBuffer : public GLBuffer
{
public:

    VertexBuffer()
        : GLBuffer(GL_ARRAY_BUFFER, GL_STATIC_DRAW)
    {

    }

    virtual ~VertexBuffer()
    {

    }
};
