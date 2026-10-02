#include"Connection.h"
Connection& ConnectionTable::GetOrCreate(int fd)
{
	std::lock_guard<std::mutex> lock(m_mutex);
	auto it = m_table.find(fd);
	if (it == m_table.end())
	{
		Connection conn;
		conn.fd = fd;
		it = m_table.emplace(fd, std::move(conn)).first;
	}
	return it->second;
}

Connection* ConnectionTable::find(int fd)
{
	std::lock_guard<std::mutex> lock(m_mutex);
	auto it = m_table.find(fd);
	return it == m_table.end() ? nullptr : &it->second;
}

void ConnectionTable::erase(int fd)
{
	std::lock_guard<std::mutex> lock(m_mutex);
	m_table.erase(fd);
}