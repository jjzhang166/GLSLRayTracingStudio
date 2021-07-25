#pragma once

#include "Common/Common.h"

#include <string>
#include <glad/glad.h>

class GLTexture
{
public:
    GLTexture(GLuint target, GLint internalformat, GLenum format, GLenum type, int32 width, int32 height, int32 depth, void* data = nullptr);
    
    virtual ~GLTexture();
    
    FORCEINLINE GLuint GetTexture()
    {
        return m_Object;
    }
    
    FORCEINLINE GLuint GetTarget()
    {
        return m_Target;
    }
    
    FORCEINLINE GLint GetInternalFormat()
    {
        return m_InternalFormat;
    }
    
    FORCEINLINE GLenum GetFormat()
    {
        return m_Format;
    }
    
    FORCEINLINE GLenum GetType()
    {
        return m_Type;
    }
    
    FORCEINLINE int32 GetWidth()
    {
        return m_Width;
    }
    
    FORCEINLINE int32 GetHeight()
    {
        return m_Height;
    }
    
    FORCEINLINE int32 GetDepth()
    {
        return m_Depth;
    }
    
    FORCEINLINE void Active()
    {
        glBindTexture(m_Target, m_Object);
    }
    
    FORCEINLINE void Deactive()
    {
        glBindTexture(m_Target, 0);
    }
    
    void SubImage2D(GLint level, GLint xoffset, GLint yoffset, GLsizei width, GLsizei height, const void* data);
    
    void Filter(GLuint minFilter, GLuint magFilter);
    
    void Wrap(GLuint s, GLuint t, GLuint r);
    
private:
    
    GLuint  m_Object;
    GLuint  m_Target;
    GLint   m_InternalFormat;
    GLenum  m_Format;
    GLenum  m_Type;
    
    int32   m_Width;
    int32   m_Height;
    int32   m_Depth;
};
