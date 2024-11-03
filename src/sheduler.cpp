#include "sheduler.h"

void TaskSheduler::AddTask(std::function<void()> task, std::time_t timer) {
    std::lock_guard<std::mutex> lock(Mutex);
    Tasks.emplace(timer, std::move(task));
    Cv.notify_one();
}

void TaskSheduler::workerThread() {
    while (true) {
        Task CurrentTask;

        {
            std::unique_lock<std::mutex> lock(Mutex);
            Cv.wait(lock, [this] {
                return !Tasks.empty() || Stop.load();
            });

            if (Stop.load() && Tasks.empty()) {
                return;
            }

            auto currentTime = std::time(nullptr);
            if (Tasks.top().first <= currentTime) {
                CurrentTask = Tasks.top();
                Tasks.pop();
            }
            else {
                Cv.wait_until(lock, std::chrono::system_clock::from_time_t(Tasks.top().first));
                continue;
            }
        }

        CurrentTask.second();
    }
}

