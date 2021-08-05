#include "Common/Log.h"
#include "Common/Common.h"
#include "Core/Program.h"

GLProgram::GLProgram(const GLShaderArray& shaders)
{
    m_Object = glCreateProgram();
    for (size_t i = 0; i < shaders.size(); ++i)
    {
        glAttachShader(m_Object, shaders[i]->Object());
    }
        
    glLinkProgram(m_Object);
    LOGI("Linking program %d\n", int32(m_Object));
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
