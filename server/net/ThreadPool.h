#pragma once
#include<iostream>
#include<thread>
#include<vector>
#include<queue>
#include<mutex>
#include<functional>
#include<condition_variable>
#include<cstdio>
class ThreadPool
{
public:
	ThreadPool(int threads_num);
	~ThreadPool();
	void AddTasks(std::function<void()> task);
	void CreateThreads(int threads_num);
private:
	std::queue<std::function<void()>> tasks;
	std::vector<std::thread> mythread;
	std::mutex task_mutex;
	std::condition_variable task_cv;
	bool stop;
};