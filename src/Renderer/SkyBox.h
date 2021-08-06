#pragma once

#include "Common/Common.h"
#include "Core/Program.h"
#include "Base/Buffer.h"
#include "Base/Base.h"

#include <glad/glad.h>

class IBLSampler;

class SkyBox
{
public:

    SkyBox();

    virtual ~SkyBox();

    void Init();

    void Destroy();

    void Draw(CameraPtr camera, IBLSampler* ibl);

private:

    GLProgram*      m_Program;
    VertexBuffer*   m_VertexBuffer;
    IndexBuffer*    m_IndexBuffer;
    GLuint          m_VAO;
};
