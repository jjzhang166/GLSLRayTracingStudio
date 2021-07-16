#pragma once

#include <vector>
#include <mutex>
#include <condition_variable>

#include "Math/Math.h"

class TaskThread;
class ThreadTask;

class TaskThreadPool
{
public:

    TaskThreadPool();

    virtual ~TaskThreadPool();

    virtual bool Create(uint32 numThreads);

    virtual void Destroy();

    virtual void AddTask(ThreadTask* task);

    virtual bool RetractTask(ThreadTask* task);

    virtual ThreadTask* ReturnToPoolOrGetNextJob(TaskThread* thread);

    static TaskThreadPool* Allocate();

    int32 GetNumQueuedJobs() const
    {
        return (int32)m_QueuedTask.size();
    }

    int32 GetNumThreads() const
    {
        return (int32)m_AllThreads.size();
    }

protected:

    std::vector<ThreadTask*>		m_QueuedTask;
    std::vector<TaskThread*>		m_QueuedThreads;
    std::vector<TaskThread*>		m_AllThreads;

    std::mutex						m_SynchMutex;
    bool							m_TimeToDie = false;

};
