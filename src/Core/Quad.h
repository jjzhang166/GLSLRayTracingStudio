#pragma once

#include "Core/Program.h"

#include <glad/glad.h>

class Quad
{
public:

    Quad();

    virtual ~Quad();

    void Draw(GLProgram* program);

private:

    GLuint m_Vao;
    GLuint m_Vbo;
};
