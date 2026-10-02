#include"MysqlPool.h"

MysqlPool::MysqlPool(int poolsize):m_poolsize(poolsize)
{
	init(poolsize);
}

MysqlPool::~MysqlPool()
{
	std::lock_guard<std::mutex> lock(m_mutex);
	running = false;
	while (!m_pool.empty())
	{
		delete m_pool.front();
		m_pool.pop();
	}
	m_cv.notify_all();
}

bool MysqlPool::init(size_t poolsize)
{
	std::unordered_map<std::string, std::string> configmap = ConfigManager::ReadMysqlConfig();
	std::string host = configmap["HOST"];
	int port = std::stoi(configmap["PORT"]);
	std::string username = configmap["USERNAME"];
	std::string password = configmap["PASSWORD"];
	std::string database = configmap["DATABASE"];
	m_poolsize = poolsize;
	for (int i = 0; i < poolsize; i++)
	{
		MysqlClient* c = new MysqlClient();
		if (!c->Connect(host, port, username, password, database))
		{
			delete c;
			std::cout << "MySQL connect failed" << std::endl;
			return false;
		}
		m_pool.push(c);
	}
	return true;
}

MysqlClient* MysqlPool::Acquire()
{
	std::unique_lock<std::mutex> lock(m_mutex);
	m_cv.wait(lock, [this]() {return !m_pool.empty() || !running; });
	if (!running || m_pool.empty())
		return nullptr;
	MysqlClient* c = m_pool.front();
	m_pool.pop();
	return c;
}

void MysqlPool::Release(MysqlClient* client)
{
	if (!client)return;
	std::lock_guard<std::mutex> lock(m_mutex);
	if (!running)
	{
		delete client;
		return;
	}
	m_pool.push(client);
	m_cv.notify_one();
}