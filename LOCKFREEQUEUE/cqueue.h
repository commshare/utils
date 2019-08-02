#pragma once

#include <queue>
#include <mutex>
#include <condition_variable>
#include <memory>
#include <chrono>
#include <cassert>

template <typename EntryType>
class CQueue {

public:
    void Push(std::unique_ptr<EntryType> item)
    {
        {
            std::lock_guard<std::mutex> lock(mutex_);
            msg_.emplace(std::move(item));
        }

        cv_.notify_one();
    }

    std::unique_ptr<EntryType> Pop()
    {
        {
            std::unique_lock<std::mutex> lock(mutex_);
            while (msg_.empty()) {
                cv_.wait(lock, [&]() {
                    return !msg_.empty();
                });
            }

            assert(false == msg_.empty());
            auto item = move(msg_.front());
            msg_.pop();
            return item;
        }
    }

    std::unique_ptr<EntryType> Pop(std::chrono::microseconds timeout)
    {
        auto time_point = std::chrono::system_clock::now() + timeout;
        {
            std::unique_lock<std::mutex> lock(mutex_);
            if (false == 
                    cv_.wait_until(lock, time_point, 
                        [&]() {
                            return !msg_.empty();
                        })) {
                // timeout
                return nullptr;
            }

            assert(false == msg_.empty());
            auto item = move(msg_.front());
            msg_.pop();
            return item;
        }
    }

private:
    std::mutex mutex_;
    std::condition_variable cv_;
    std::queue<std::unique_ptr<EntryType>> msg_;
};




