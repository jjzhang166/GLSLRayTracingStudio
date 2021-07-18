#pragma once

#include "Math/WindowsPlatformAtomics.h"

class ThreadTask
{
private:

    enum Status
    {
        None = 0,
        Done = 1
    };

public:

    ThreadTask()
        : m_Status((int32)Status::None)
    {

    }

    virtual ~ThreadTask() 
    {

    }

    virtual void DoThreadedWork() = 0;

    virtual void Abandon() = 0;

    virtual bool IsDone() const
    {
        return PlatformAtomics::AtomicRead(&m_Status) == (int32)Status::Done;
    }

    virtual void OnComplete()
    {
        PlatformAtomics::InterlockedExchange(&m_Status, (int32)Status::Done);
    }

protected:

    volatile int32 m_Status;
};
