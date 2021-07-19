#pragma once

#include "Base/Base.h"
#include "Job/ThreadTask.h"
#include "Math/WindowsPlatformAtomics.h"

class LoadGLTFJob : public ThreadTask
{
public:

    LoadGLTFJob(const std::string& path);

    virtual ~LoadGLTFJob();

    virtual void DoThreadedWork() override;

    virtual void Abandon() override
    {

    }

    FORCEINLINE Scene3DPtr GetScene() const
    {
        return m_Scene3D;
    }

private:

    std::string         m_Path;
    Scene3DPtr          m_Scene3D;
};
