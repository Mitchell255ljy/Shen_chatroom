#include"UserDao.h"
#include"MysqlClient.h"
#include<mysql/mysql.h>
UserDao::UserDao(MysqlClient* client) :m_client(client) {}
bool UserDao::FindByUsername(int username, User& out)
{
	std::string sql = "SELECT username,nickname,password FROM user_information WHERE username=" + std::to_string(username) + " LIMIT 1";
	MYSQL_RES* res = m_client->Query(sql);
	if (!res)return false;
	MYSQL_ROW row = mysql_fetch_row(res);
	if (!row)
	{
		mysql_free_result(res);
		return false;
	}
	out.username = std::stoi(row[0]);
	out.nickname = row[1];
	out.password = row[2];
	out.online = false;
	mysql_free_result(res);
	return true;
}
bool UserDao::InsertUser(const int username, const std::string nickname, const std::string password)
{
	std::string safenickname = m_client->Escape(nickname);
	std::string safepassword = m_client->Escape(password);
	std::string sql = "INSERT INTO user_information(username,nickname,password) VALUES(" + std::to_string(username) + ",'" + safenickname + "','" + safepassword + "')";
	if (!m_client->Execute(sql))return false;
	return true;
}
bool UserDao::ExistsByUsername(int username)
{
	std::string sql = "SELECT username FROM user_information WHERE username=" + std::to_string(username) + " LIMIT 1";
	MYSQL_RES* res = m_client->Query(sql);
	if (!res)return false;
	bool exists = (mysql_fetch_row(res) != nullptr);
	mysql_free_result(res);
	return exists;
}