#include "Executor.h"

#include <algorithm>
#include <thread>
#include <condition_variable>
#include <vector>

#include "ThreadSafeRingBuffer.h"

namespace Executor
{
    //Executor context
    bool s_IsRunning = false;
    ThreadSafeRingBuffer<std::function<void()>, 256> s_Jobs;
    std::condition_variable s_WakeCondition;
    std::mutex s_WakeMutex;
    std::atomic_uint64_t s_SubmittedTasks;
    std::atomic_uint64_t s_ExecutedTasks;

    class WorkerThread
    {
    public:
        void Initialize()
        {
            m_Thread = std::thread(&WorkerThread::Execute, this);
        }

        void Execute()
        {
            while(s_IsRunning)
            {
                std::function<void()> job;
                if (s_Jobs.pop_front(job))
                {
                    job();
                    s_ExecutedTasks.fetch_add(1);
                }
                else
                {
                    std::unique_lock<std::mutex> lock(s_WakeMutex);
                    s_WakeCondition.wait(lock);
                }
            }
        }

        void Join()
        {
            m_Thread.join();
        }

    private:
        std::thread m_Thread;
    };

    std::vector<WorkerThread> s_Workers;


    void Initialize()
    {
        uint32_t systemCoreCount = std::thread::hardware_concurrency();
        uint32_t threadCount = std::max(1u, systemCoreCount);

        for (uint32_t i = 0; i < threadCount; i++)
        {
            WorkerThread& worker = s_Workers.emplace_back();
            worker.Initialize();
        }

        s_SubmittedTasks.store(0);
        s_ExecutedTasks.store(0);
    }

    void Execute(const std::function<void()>& job)
    {
        //Push back one Task
        while (!s_Jobs.push_back(job))
        {
            // While pushing fails, wake a Thread to work then yield
            s_WakeCondition.notify_one();
            std::this_thread::yield();
        }

        s_SubmittedTasks.fetch_add(1);

        // Notify a Thread to execute the pushed task
        s_WakeCondition.notify_one();
    }

    bool IsBusy()
    {
        return s_ExecutedTasks.load() < s_SubmittedTasks.load();
    }

    void Wait()
    {

    }
}