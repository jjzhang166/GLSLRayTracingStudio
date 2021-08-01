#pragma once

#include <memory>
#include <vector>
#include <string>
#include <glad/glad.h>

class GLShader
{
public:

    GLShader(const std::string& filePath, GLuint shaderType);

    virtual ~GLShader();

    GLuint Object() const;

private:

    GLuint m_Object;
};

typedef std::shared_ptr<GLShader> GLShaderPtr;
typedef std::vector<GLShaderPtr>  GLShaderArray;
