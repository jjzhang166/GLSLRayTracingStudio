#pragma once

#include "Core/Shader.h"

#include <memory>
#include <vector>
#include <string>

class GLProgram
{
public:

    GLProgram(const GLShaderArray& shaders);

    virtual ~GLProgram();

    void Active();

    void Deactive();

    GLuint Object();

private:

    GLuint m_Object;
};

typedef std::shared_ptr<GLProgram> GLProgramPtr;
typedef std::vector<GLProgramPtr>  GLProgramArray;
