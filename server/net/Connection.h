#pragma once

#include<string>
#include<unordered_map>
#include<mutex>
#include<cstddef>
#include<sys/types.h>

struct Connection
{
	int fd = -1;
	std::string write_buf;
	std::string read_buf;
	bool want_write = false;

	void AppendWrite(const char* data,ssize_t n)
	{
		write_buf.append(data, n);
	}
	void AppendRead(const char* data, ssize_t n)
	{
		read_buf.append(data, n);
	}
};

class ConnectionTable
{
public:
	Connection& GetOrCreate(int fd);
	Connection* find(int fd);
	void erase(int fd);

private:
	std::unordered_map<int, Connection> m_table;
	std::mutex m_mutex;
};