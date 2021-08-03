#include "Base/Buffer.h"

GLBuffer::GLBuffer(GLenum target, GLuint usage)
    : m_Target(target)
    , m_Usage(usage)
    , m_Buffer(0)
{

}

GLBuffer::~GLBuffer()
{
    if (m_Buffer != 0)
    {
        glDeleteBuffers(1, &m_Buffer);
        m_Buffer = 0;
    }
}

void GLBuffer::Active()
{
    glBindBuffer(m_Target, m_Buffer);
}

void GLBuffer::Deactive()
{
    glBindBuffer(m_Target, 0);
}

void GLBuffer::Upload(uint8* bytes, int32 length)
{
    if (m_Buffer == 0)
    {
        glGenBuffers(1, &m_Buffer);
    }

    m_Length = length;

    glBindBuffer(m_Target, m_Buffer);
    glBufferData(m_Target, m_Length, bytes, m_Usage);
}
