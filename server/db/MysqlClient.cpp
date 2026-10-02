#include"MysqlClient.h"

MysqlClient::MysqlClient() :m_conn(nullptr)
{
	m_conn = mysql_init(nullptr);
	if (!m_conn)
	{
		std::cerr << "mysql_init error: " << mysql_error(m_conn) << std::endl;
		exit(1);
	}
}
MysqlClient::~MysqlClient()
{
	if (m_conn)
	{
		mysql_close(m_conn);
		m_conn = nullptr;
	}
}

bool MysqlClient::Connect(const std::string& host, int port, const std::string& username,
	const std::string& password, const std::string& db)
{
	if (!mysql_real_connect(m_conn, host.c_str(), username.c_str(), password.c_str(), db.c_str(), port, nullptr, 0))
	{
		std::cerr << "mysql_real_connect error: " << mysql_error(m_conn) << std::endl;
		return false;
	}
	return true;
}

MYSQL_RES* MysqlClient::Query(const std::string& sql)
{
	std::lock_guard<std::mutex> lock(m_mutex);
	if (mysql_query(m_conn, sql.c_str()) != 0)
	{
		std::cerr << "mysql_query error: " << mysql_error(m_conn) << std::endl;
		return nullptr;
	}
	return mysql_store_result(m_conn);
}

bool MysqlClient::Execute(const std::string& sql)
{
	std::lock_guard<std::mutex> lock(m_mutex);
	if (mysql_query(m_conn, sql.c_str()) != 0)
	{
		std::cerr << "mysql_query error: " << mysql_error(m_conn) << std::endl;
		return false;
	}
	return true;
}

std::string MysqlClient::Escape(const std::string& input)
{
	std::string out;
	out.resize(input.size() * 2 + 1);
	unsigned long len = mysql_real_escape_string(m_conn, &out[0], input.c_str(), input.size());
	out.resize(len);
	return out;
}