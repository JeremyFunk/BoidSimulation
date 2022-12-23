#pragma once

#include <iostream>
#include <vector>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <queue>
#include <functional>

class ThreadPool
{
public:
    ThreadPool(int numThreads)
    {
        for (int i = 0; i < numThreads; i++) {
            m_threads.emplace_back([&] {
                while (true) {
                    std::function<void()> task;
                    {
                        std::unique_lock<std::mutex> lock(m_mutex);
                        m_cv.wait(lock, [&] { return m_stop || !m_tasks.empty(); });
                        if (m_stop && m_tasks.empty()) {
                            return;
                        }
                        task = std::move(m_tasks.front());
                        m_tasks.pop();
                    }
                    task();
                }
                });
        }
    }

    template <typename F, typename... Args>
    void enqueue(F&& f, Args&&... args)
    {
        {
            std::unique_lock<std::mutex> lock(m_mutex);
            m_tasks.emplace([&] {
                f(args...);
                });
        }
        m_cv.notify_one();
    }

    void stop()
    {
        {
            std::unique_lock<std::mutex> lock(m_mutex);
            m_stop = true;
        }
        m_cv.notify_all();
        for (auto& thread : m_threads) {
            thread.join();
        }
    }

private:
    std::vector<std::thread> m_threads;
    std::mutex m_mutex;
    std::condition_variable m_cv;
    std::queue<std::function<void()>> m_tasks;
    bool m_stop = false;
};
