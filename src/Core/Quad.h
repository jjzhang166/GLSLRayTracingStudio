#pragma once

#include "Core/Program.h"

#include <glad/glad.h>

class Quad
{
public:

    Quad();

    void Draw(GLProgramPtr program);

private:

    GLuint m_Vao;
    GLuint m_Vbo;
};
