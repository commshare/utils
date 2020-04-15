#pragma once
#include<functional>
#include <atomic>
#include<queue>
#include <thread>
#include <mutex>
#include <condition_variable>

namespace EduMediaScreen
{
  class Runnable
  {
  public:
	explicit Runnable(const std::function<void()>f);
	void run() const;
  private:
	std::function<void()>f_;
  };

  class Executor {
  public:
	Executor();
	~Executor();
	static Executor * instance();
	void execute(Runnable *task);
	bool isShutdown() const;
	bool start();
	void shutdown();
	void wakeUp();
  private:
	static Executor *self_;
	std::atomic<bool> shutdown_;
	std::mutex tasksQueueMutex_;
	std::queue<Runnable*> taskQueue_;
	std::thread thread_;
	std::condition_variable cv_;
	std::mutex cvMutext_;
  };
}
