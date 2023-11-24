#pragma once

#include <functional>

namespace Executor
{
    /// @brief Create the internal resources such as worker threads, etc...
    void Initialize();

    /// @brief Adds a job to be executed asynchronously. An idle thread will execute it.
    /// @param job 
    void Execute(const std::function<void()>& job);

    /// @brief Checks if any thread is busy
    /// @return true if at least one thread is busy 
    bool IsBusy();

    /// @brief Waits until all threads are idle
    void Wait();
}