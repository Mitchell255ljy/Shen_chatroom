#pragma once
#include<string>
#include<vector>
#include<queue>
#include<mutex>
#include<condition_variable>
#include<atomic>
#include<unordered_map>
#include<mysql/mysql.h>
#include<iostream>

#include"MysqlClient.h"
#include"../util/ConfigManager.h"

class MysqlPool
{
public:
	MysqlPool(int poolsize);
	~MysqlPool();

	bool init(size_t poolsize);
	MysqlClient* Acquire();
	void Release(MysqlClient* client);

private:
	std::queue<MysqlClient*> m_pool;
	std::mutex m_mutex;
	std::condition_variable m_cv;
	size_t m_poolsize;
	std::atomic<bool> running{ true };
};