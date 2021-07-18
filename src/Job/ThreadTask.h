#pragma once

#include "Math/WindowsPlatformAtomics.h"

#include <functional>

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
        , onCompleteEvent(nullptr)
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

public:

    std::function<void(ThreadTask*)>    onCompleteEvent;

protected:

    volatile int32 m_Status;
};
