#include"ThreadPool.h"
ThreadPool::ThreadPool(int threads_num) :stop(false)
{
	CreateThreads(threads_num);
}
ThreadPool::~ThreadPool()
{
	{
		std::lock_guard<std::mutex> lock(task_mutex);
		stop = true;
	}
	task_cv.notify_all();
	for (auto& i : mythread)
	{
		i.join();
	}
}
void ThreadPool::AddTasks(std::function<void()> task)
{
	{
		std::lock_guard<std::mutex> lock(task_mutex);
		tasks.push(task);
	}
	task_cv.notify_one();
}
void ThreadPool::CreateThreads(int threads_num)
{
	for (int i = 0; i < threads_num; i++)
	{
		mythread.emplace_back([this] {
			while (true)
			{
				std::function<void()> task;
				{
					std::unique_lock<std::mutex> lock(task_mutex);
					task_cv.wait(lock, [this] {return stop || !(tasks.empty()); });
					if (stop && tasks.empty())
					{
						return;
					}
					task = std::move(tasks.front());
					tasks.pop();
				}
				task();
			}
			});
	}
}