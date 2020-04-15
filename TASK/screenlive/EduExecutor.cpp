#include "stdafx.h"
#include "EduExecutor.h"

namespace EduMediaScreen
{
  Runnable::Runnable(const std::function<void()>f)
	: f_(f)
  {

  }
  void Runnable::run() const {
	f_();
  }

  Executor * Executor::self_ = nullptr;
  Executor::Executor()
	: cvMutext_()
	, shutdown_(true)
	, taskQueue_()
	, tasksQueueMutex_()
	, thread_()
  {
	self_ = this;
  }
  Executor::~Executor()
  {
	self_ = nullptr;
	shutdown();
  }
  Executor * Executor::instance()
  {
	return self_;
  }
  bool Executor::start()
  {
	if (!isShutdown())
	{
	  return true;
	}
	auto threadMain = [this]() {
	  while (!shutdown_)
	  {
		while (!taskQueue_.empty()) {
		  Runnable *task = nullptr;
		  {
			std::lock_guard<std::mutex> lock(tasksQueueMutex_);
			if (!taskQueue_.empty())
			{
			  task = taskQueue_.front();
			  taskQueue_.pop();
			}
		  }
		  if (task)
		  {
			task->run();
			delete task;
		  }
		}
		if (!shutdown_)
		{
		  std::unique_lock<std::mutex> lk(cvMutext_);
		  cv_.wait(lk);
		}
	  }
	};
	shutdown_ = false;
	thread_ = std::thread(threadMain);
	return true;
  }

  bool Executor::isShutdown() const {
	return shutdown_;
  }

  void Executor::execute(Runnable *task)
  {
	if (isShutdown())
	{
	  return;
	}
	std::lock_guard<std::mutex> locker(tasksQueueMutex_);
	taskQueue_.push(task);
	wakeUp();
  }

  void Executor::wakeUp()
  {
	std::unique_lock<std::mutex> lk(cvMutext_);
	cv_.notify_one();
  }
  void Executor::shutdown()
  {
	if (isShutdown())
	{
	  return;
	}
	{
	  std::lock_guard<std::mutex> locker(tasksQueueMutex_);
	  while (taskQueue_.size() > 0)
	  {
		auto task = taskQueue_.front();
		delete task;
		taskQueue_.pop();
	  }
	}
	shutdown_ = true;
	wakeUp();
	if (thread_.joinable())
	{
	  thread_.join();
	}
  }

}