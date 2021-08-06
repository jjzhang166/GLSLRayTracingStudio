#include "Common/Log.h"

#include "Core/Program.h"

GLProgram::GLProgram(const std::vector<std::shared_ptr<GLShader>>& shaders)
{
    m_Object = glCreateProgram();
    for (size_t i = 0; i < shaders.size(); ++i)
    {
        glAttachShader(m_Object, shaders[i]->Object());
    }
        
    glLinkProgram(m_Object);
    for (size_t i = 0; i < shaders.size(); ++i)
    {
        glDetachShader(m_Object, shaders[i]->Object());
    }

    GLint success = 0;
    glGetProgramiv(m_Object, GL_LINK_STATUS, &success);
    if (success == GL_FALSE)
    {
        std::string msg("Error while linking program\n");
        GLint logSize = 0;
        glGetProgramiv(m_Object, GL_INFO_LOG_LENGTH, &logSize);
        char* info = new char[logSize + 1];
        glGetShaderInfoLog(m_Object, logSize, NULL, info);
        info[logSize] = '\0';
        msg += info;
        delete[] info;
        glDeleteProgram(m_Object);
        m_Object = 0;
        LOGE("%s\n", msg.c_str());
    }
}

GLProgram::~GLProgram()
{
    glDeleteProgram(m_Object);
}

void GLProgram::Active()
{
    glUseProgram(m_Object);
}

void GLProgram::Deactive()
{
    glUseProgram(0);
}

GLuint GLProgram::Object()
{
    return m_Object;
}

void GLProgram::SetUniform1i(const char* name, int32 val)
{
    GLint loc = glGetUniformLocation(m_Object, name);
    if (loc >= 0)
    {
        glUniform1i(loc, val);
    }
}

void GLProgram::SetUniform1f(const char* name, float val)
{
    GLint loc = glGetUniformLocation(m_Object, name);
    if (loc >= 0)
    {
        glUniform1f(loc, val);
    }
}

void GLProgram::SetUniform2f(const char* name, const Vector2& val)
{
    GLint loc = glGetUniformLocation(m_Object, name);
    if (loc >= 0)
    {
        glUniform2f(loc, val.x, val.y);
    }
}

void GLProgram::SetUniform3f(const char* name, const Vector3& val)
{
    GLint loc = glGetUniformLocation(m_Object, name);
    if (loc >= 0)
    {
        glUniform3f(loc, val.x, val.y, val.z);
    }
}

void GLProgram::SetUniform4f(const char* name, const Vector4& val)
{
    GLint loc = glGetUniformLocation(m_Object, name);
    if (loc >= 0)
    {
        glUniform4f(loc, val.x, val.y, val.z, val.w);
    }
}

void GLProgram::SetUniform4x4f(const char* name, const Matrix4x4& val)
{
    GLint loc = glGetUniformLocation(m_Object, name);
    if (loc >= 0)
    {
        glUniformMatrix4fv(loc, 1, GL_FALSE, &(val.m[0][0]));
    }
}

void GLProgram::SetTexture(const char* name, GLenum target, GLuint texture, int32 slot)
{
    GLint loc = glGetUniformLocation(m_Object, name);
    if (loc >= 0)
    {
        glActiveTexture(GL_TEXTURE0 + slot);
        glBindTexture(target, texture);
        glUniform1i(loc, slot);
    }
}
