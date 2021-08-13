#pragma once

#include "Common/Common.h"

#include "Base/Base.h"
#include "Base/Buffer.h"

#include "Core/Quad.h"
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

    FORCEINLINE HDRImagePtr HDRImage() const
    {
        return m_HDRImage;
    }

    FORCEINLINE GLuint HDRTexture() const
    {
        return m_InputTexture;
    }

    FORCEINLINE GLuint Background() const
    {
        return m_CubeTexture;
    }

    FORCEINLINE GLuint LambertTexture() const
    {
        return m_LambertianTexture;
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

public:

    float           exposure = 1.0f;
    float           gammaValue = 2.0f;
    float           environmentLod = 0.0f;
    float           environmentIntensity = 1.0f;

private:

    GLuint CreateCubemapTexture(bool withMipmaps);

    void CubeMapToLambertian();

    void PanoramaToCubeMap();

    void CubeMapToGGX();

    void CubeMapToSheen();

    void ApplyFilter(int32 distribution, float roughness, int32 targetMipLevel, GLuint targetTexture, int32 sampleCount);

private:

    GLuint          m_InputTexture;
    GLuint          m_CubeTexture;

    GLuint          m_LambertianTexture;
    GLuint          m_GGXTexture;
    GLuint          m_SheenTexture;

    GLuint          m_GGXLutTexture;
    GLuint          m_CharlieLutTexture;

    GLuint          m_FrameBuffer;

    GLProgram*      m_ProgramIBL;
    GLProgram*      m_ProgramCube;
    Quad*           m_Quad;

    int32           m_TextureSize;
    int32           m_GGXSampleCount;
    int32           m_LambertianSampleCount;
    int32           m_SheenSamplCount;
    float           m_LodBias;
    int32           m_LutResolution;
    int32           m_LowestMipLevel;
    int32           m_MipMapCount;

    HDRImagePtr     m_HDRImage;
    
};
