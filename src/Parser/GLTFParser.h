#pragma once

#include "Base/Base.h"
#include "Job/ThreadTask.h"
#include "Math/WindowsPlatformAtomics.h"

class JobLoadGLTF : public ThreadTask
{
public:

    JobLoadGLTF(const std::string& path);

    virtual ~JobLoadGLTF();

    virtual void DoThreadedWork() override;

    virtual void Abandon() override
    {

    }

    FORCEINLINE float GetProgress()
    {
        int32 total = PlatformAtomics::AtomicRead(&m_ProgressTotal);
        int32 count = PlatformAtomics::AtomicRead(&m_ProgressCount);
        return count * 1.0f / total;
    }

private:

    std::string         m_Path;
    Scene3DPtr          m_Scene3D;
    volatile int32      m_ProgressTotal;
    volatile int32      m_ProgressCount;
};
