#pragma once

#include "Common/Common.h"

#include "Base/Base.h"
#include "Base/Buffer.h"

#include "Core/Program.h"
#include "Core/Texture.h"

#include <glad/glad.h>

class IBLSampler
{

public:

    IBLSampler();

    virtual ~IBLSampler();

    void Init(HDRImagePtr hdrImage);

    void Destroy();

    FORCEINLINE GLuint Background() const
    {
        return m_CubeTexture;
    }

    FORCEINLINE GLuint LambertTexture() const
    {
        return m_LambertTexture;
    }

    FORCEINLINE GLuint GGXTexture() const
    {
        return m_GGXTexture;
    }

    FORCEINLINE GLuint SheenTexture() const
    {
        return m_SheenTexture;
    }

    FORCEINLINE int32 MipmapLevels() const
    {
        return m_MipMapCount;
    }

    FORCEINLINE GLuint FrameBuffer() const
    {
        return m_FrameBuffer;
    }

    FORCEINLINE float LodBias() const
    {
        return m_LodBias;
    }

    FORCEINLINE int32 SampleCount() const
    {
        return m_SampleCount;
    }

    FORCEINLINE int32 SampleSize() const
    {
        return m_SampleSize;
    }

    void PanoramaToCubeMap();

private:

    GLuint CreateCubemapTexture(bool withMipmaps, int32 size);

    void CubeMapToLambertian();

    void CubeMapToGGX();

    void CubeMapToSheen();

    void Draw();

    void ApplyFilter(int32 distribution, float roughness, int32 targetMipLevel, GLuint targetTexture);

private:

    VertexBuffer*   m_VertexBuffer;
    IndexBuffer*    m_IndexBuffer;

    GLuint          m_InputTexture;
    GLuint          m_CubeTexture;
    GLuint          m_LambertTexture;
    GLuint          m_GGXTexture;
    GLuint          m_SheenTexture;
    GLuint          m_FrameBuffer;
    GLuint          m_VAO;

    GLProgram*      m_ProgramIBL;
    GLProgram*      m_ProgramCube;

    int32           m_SampleSize;
    int32           m_SampleCount;
    float           m_LodBias;
    int32           m_MipMapCount;

};
