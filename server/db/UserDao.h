#pragma once
#include<string>
#include"MysqlClient.h"

struct User
{
	int username;
	std::string nickname;
	std::string password;
	bool online = false;
};

class UserDao
{
public:
	explicit UserDao(MysqlClient* client);
	bool FindByUsername(int username, User& out);
	bool InsertUser(const int username, const std::string nickname, const std::string password);
	bool ExistsByUsername(int username);

private:
	MysqlClient* m_client;
};