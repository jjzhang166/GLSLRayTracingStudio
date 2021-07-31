#pragma once

#include "Common/Common.h"
#include "Job/ThreadTask.h"
#include "Job/TaskThreadPool.h"

class JobManager
{
private:

    JobManager();

    ~JobManager();

public:

    static void Init(int32 threadNum);

    static void Destroy();

    static void AddJob(ThreadTask* task);

    static void Tick();

    static int32 Count();

    static TaskThreadPool* TaskPool();
};
