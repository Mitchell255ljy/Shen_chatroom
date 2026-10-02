#pragma once

#include<mysql/mysql.h>
#include<string>
#include<mutex>
#include<iostream>

class MysqlClient
{
public:
	MysqlClient();
	~MysqlClient();

	bool Connect(const std::string& host, int port, const std::string& username,
		const std::string& password, const std::string& db);
	MYSQL_RES* Query(const std::string& sql);
	bool Execute(const std::string& sql);
	std::string Escape(const std::string& input);
	MYSQL* Raw()
	{
		return m_conn;
	}

private:
	MYSQL* m_conn;
	std::mutex m_mutex;
};