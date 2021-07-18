#include "Misc/JobManager.h"
#include "Math/Math.h"

static TaskThreadPool*          s_TaskPool = nullptr;
static std::vector<ThreadTask*> s_Jobs;

JobManager::JobManager()
{

}

JobManager::~JobManager()
{

}

int32 JobManager::Count()
{
    return int32(s_Jobs.size());
}

void JobManager::Tick()
{
    if (s_Jobs.size() == 0)
    {
        return;
    }

    for (int32 i = (int32)s_Jobs.size() - 1; i >= 0; --i)
    {
        ThreadTask* job = s_Jobs[i];
        if (job->IsDone())
        {
            if (job->onCompleteEvent)
            {
                job->onCompleteEvent(job);
                job->onCompleteEvent = nullptr;
            }
            delete job;
            s_Jobs.erase(s_Jobs.begin() + i);
        }
    }
}

void JobManager::Init(int32 threadNum)
{
    if (s_TaskPool)
    {
        return;
    }

    s_TaskPool = new TaskThreadPool();
    s_TaskPool->Create(MMath::Max((int32)std::thread::hardware_concurrency(), 8));
}

void JobManager::Destroy()
{
    if (s_TaskPool == nullptr)
    {
        return;
    }

    {
        s_TaskPool->Destroy();
        delete s_TaskPool;
        s_TaskPool = nullptr;
    }

    {
        for (size_t i = 0; i < s_Jobs.size(); ++i)
        {
            delete s_Jobs[i];
        }
        s_Jobs.clear();
    }
}

void JobManager::AddJob(ThreadTask* task)
{
    if (task == nullptr)
    {
        return;
    }

    if (s_TaskPool)
    {
        s_Jobs.push_back(task);
        s_TaskPool->AddTask(task);
    }
    else
    {
        task->DoThreadedWork();
        task->OnComplete();
    }
}
