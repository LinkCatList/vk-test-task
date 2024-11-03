#include <condition_variable>
#include <iostream>
#include <atomic>
#include <mutex>
#include <thread>
#include <functional>
#include <queue>

class TaskSheduler {
    public:
        TaskSheduler()
            : Stop(false)
        {
            Worker = std::thread(&TaskSheduler::workerThread, this);
        }
    
        void AddTask(std::function<void()> task, std::time_t timer);

        ~TaskSheduler() {
            Stop.store(true);
            Cv.notify_all();
            if (Worker.joinable()) {
                Worker.join();
            }
        }

    private:
        using Task = std::pair<std::time_t, std::function<void()>>;
        class Compare {
            public:
                bool operator()(const Task& lhs, const Task& rhs) {
                    return lhs.first > rhs.first;
                }
        };
    
    private:
        void workerThread();

    private:
        std::priority_queue<Task, std::vector<Task>, Compare> Tasks;
        std::atomic<bool> Stop;
        std::thread Worker;
        std::condition_variable Cv;
        std::mutex Mutex;
};
