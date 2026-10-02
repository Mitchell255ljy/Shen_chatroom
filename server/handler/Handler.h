#pragma once
#include<iostream>
#include<string>
#include<vector>
#include<ctime>

#include"../net/Connection.h"
#include"../net/SessionManager.h"
#include"../third_party/json.hpp"
#include"../db/MysqlPool.h"
#include"../db/MysqlClient.h"
#include"../db/UserDao.h"
#include"../net/SendQueue.h"
#include"../../common/message_schema.h"
#include"../../common/protocol.h"
#include"../protocol/Protocol.h"

using json = nlohmann::json;

class Handler
{
public:
	Handler(SessionManager* sessionmgr,MysqlPool* mysqlpool,SendQueue* sendqueue);
	void LoginHandle(Connection* conn, const std::string& body);
	void RegisterHandle(Connection* conn, const std::string& body);
	void ChatHandle(Connection* conn, const std::string& body);
	void PrivateChatHandle(Connection* conn, const std::string& body);
	void UserListHandle(Connection* conn, const std::string& body);
	void LogoutHandle(Connection* conn, const std::string& body);
	void SendLoginSuccess(Connection* conn, int username, const std::string& nickname, bool online);
	void SendLoginFailed(Connection* conn, const std::string& reason);
	void SendRegisterSuccess(Connection* conn);
	void SendRegisterFailed(Connection* conn, const std::string& reason);

private:
	SessionManager* m_sessionmgr;
	MysqlPool* m_mysqlpool;
	SendQueue* m_sendqueue;
};