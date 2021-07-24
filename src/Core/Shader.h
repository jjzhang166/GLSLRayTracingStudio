#pragma once

#include <string>
#include <glad/glad.h>

class GLShader
{
public:

    GLShader(const std::string& filePath, GLuint shaderType);

    GLuint Object() const;

private:

    GLuint m_Object;
};
