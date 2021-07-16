#pragma once

#include "Math/Math.h"

class RunnableThread;

class Runnable
{
public:

    Runnable()
        : m_RunnableThread(nullptr)
    {

    }

    virtual ~Runnable()
    {

    }

    virtual bool Init()
    {
        return true;
    }

    virtual int32 Run() = 0;

    virtual void Stop()
    {

    }

    virtual void Exit()
    {

    }

protected:

    friend class RunnableThread;

    RunnableThread* m_RunnableThread;
};
