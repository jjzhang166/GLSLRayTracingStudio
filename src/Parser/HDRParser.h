#pragma once

#include "Base/Base.h"
#include "Job/ThreadTask.h"

class LoadHDRJob : public ThreadTask
{
public:

    LoadHDRJob(const std::string& path);

    virtual ~LoadHDRJob();

    virtual void DoThreadedWork() override;

    virtual void Abandon() override
    {

    }

    FORCEINLINE HDRImagePtr GetHDRImage() const
    {
        return m_HDRImage;
    }

private:

    void CreateEnvImportanceTexture();

    void LoadHDRImage();

private:

    std::string     m_Path;
    HDRImagePtr     m_HDRImage;
};
