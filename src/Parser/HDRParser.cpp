#include "Parser/HDRParser.h"
#include "Misc/FileMisc.h"
#include "Base/Base.h"
#include "Math/Math.h"
#include "Parser/stb_image.h"

#include <string>
#include <algorithm>
#include <numeric>
#include <glad/glad.h>

static FORCEINLINE float Luminance(float r, float g, float b)
{
    return r * 0.2126f + g * 0.7152f + b * 0.0722f;
}

LoadHDRJob::LoadHDRJob(const std::string& path)
    : m_Path(path)
    , m_HDRImage(nullptr)
{

}

LoadHDRJob::~LoadHDRJob()
{

}

void LoadHDRJob::DoThreadedWork()
{
    LoadHDRImage();
    CreateEnvImportanceTexture();
}

void LoadHDRJob::LoadHDRImage()
{
    m_HDRImage = std::make_shared<HDRImage>();
    float* pixels = stbi_loadf(m_Path.c_str(), &(m_HDRImage->width), &(m_HDRImage->height), &(m_HDRImage->component), STBI_rgb_alpha);

    m_HDRImage->hdrRGBA.resize(m_HDRImage->width * m_HDRImage->height * m_HDRImage->component);
    memcpy(m_HDRImage->hdrRGBA.data(), pixels, m_HDRImage->hdrRGBA.size() * sizeof(float));

    stbi_image_free(pixels);
}

void LoadHDRJob::CreateEnvImportanceTexture()
{
    int32 width     = m_HDRImage->width;
    int32 height    = m_HDRImage->height;
    int32 component = m_HDRImage->component;

    struct EnvAccel
    {
        uint32  alias;
        float   q;
        float   pdf;
    };

    float cosTheta0 = 1.0f;
    float stepPhi   = 2.0f * PI / width;
    float stepTheta = PI / height;
    float total     = 0;

    std::vector<EnvAccel> envAccel(width * height);
    std::vector<float>    importanceData(width * height);

    for (int32 y = 0; y < height; ++y)
    {
        float theta1    = (y + 1) * stepTheta;
        float cosTheta1 = MMath::Cos(theta1);
        float area      = (cosTheta0 - cosTheta1) * stepPhi;
        cosTheta0       = cosTheta1;

        for (int32 x = 0; x < width; ++x)
        {
            int32 idx = y * width + x;
            float r   = m_HDRImage->hdrRGBA[idx * component + 0];
            float g   = m_HDRImage->hdrRGBA[idx * component + 1];
            float b   = m_HDRImage->hdrRGBA[idx * component + 2];

            total += Luminance(r, g, b);
            importanceData[idx] = area * MMath::Max3(r, g, b);
        }
    }

    float average  = total / width / height;
    float integral = std::accumulate(importanceData.begin(), importanceData.end(), 0.0f);

    uint32 size = uint32(importanceData.size());
    float fsize = float(size);
    for (uint32 i = 0; i < size; ++i)
    {
        envAccel[i].q = fsize * importanceData[i] / integral;
        envAccel[i].alias = i;
    }

    uint32 s = 0;
    uint32 large = size;
    std::vector<uint32> partitionTable(size);
    
    for (uint32 i = 0; i < size; ++i)
    {
        partitionTable[(envAccel[i].q < 1.0f) ? (s++) : (--large)] = i;
    }

    for (s = 0; s < large && large < size; ++s)
    {
        const uint32_t j = partitionTable[s];
        const uint32_t k = partitionTable[large];
        envAccel[j].alias = k;
        envAccel[k].q += envAccel[j].q - 1.0f;
        large = (envAccel[k].q < 1.0f) ? (large + 1u) : large;
    }

    for (int32 i = 0; i < width * height; ++i)
    {
        float r = m_HDRImage->hdrRGBA[i * component + 0];
        float g = m_HDRImage->hdrRGBA[i * component + 1];
        float b = m_HDRImage->hdrRGBA[i * component + 2];
        envAccel[i].pdf = MMath::Max3(r, g, b) * 1.0f / integral;
    }

    m_HDRImage->envRGBA.resize(envAccel.size() * 3);
    for (size_t i = 0; i < envAccel.size(); ++i)
    {
        m_HDRImage->envRGBA.push_back((float)envAccel[i].alias);
        m_HDRImage->envRGBA.push_back((float)envAccel[i].q);
        m_HDRImage->envRGBA.push_back((float)envAccel[i].pdf);
    }
}
