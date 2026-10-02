#pragma once
#include<queue>
#include<mutex>
#include<string>
#include<vector>
#include<iostream>
#include<unistd.h>
#include<cstdint>

struct SendTask
{
	int fd;
	std::string data;
};

class SendQueue
{
public:

	void SetWakeFd(int fd)
	{
		m_wakefd = fd;
	}

	void push(SendTask task)
	{
		{
			std::lock_guard<std::mutex> lock(m_mutex);
			m_queue.push(std::move(task));
		}
		if (m_wakefd >= 0)
		{
			uint64_t one = 1;
			write(m_wakefd, &one, sizeof(one));
		}
	}
	std::vector<SendTask> drain()
	{
		std::vector<SendTask> out;
		{
			std::lock_guard<std::mutex> lock(m_mutex);
			while (!m_queue.empty())
			{
				out.push_back(std::move(m_queue.front()));
				m_queue.pop();
			}
		}
		return out;
	}

private:
	int m_wakefd = -1;
	std::queue<SendTask> m_queue;
	std::mutex m_mutex;
};