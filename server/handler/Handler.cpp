#include"Handler.h"

Handler::Handler(SessionManager * sessionmgr, MysqlPool * mysqlpool,SendQueue* sendqueue):m_sessionmgr(sessionmgr),m_mysqlpool(mysqlpool),m_sendqueue(sendqueue)
{}

void Handler::LoginHandle(Connection* conn, const std::string& body)
{
	int fd = conn->fd;
	auto j = json::parse(body);
	int username = j[message_schema::USERNAME].get<int>();
	std::string password = j[message_schema::PASSWORD].get<std::string>();

	MysqlClient* mysql = m_mysqlpool->Acquire();
	UserDao dao(mysql);
	User user;
	bool ok = dao.FindByUsername(username, user);
	m_mysqlpool->Release(mysql);

	if (!ok)
	{
		SendLoginFailed(conn, "user not exists");
		return;
	}
	if (user.password != password)
	{
		SendLoginFailed(conn, "password incorrect");
		return;
	}
	if (m_sessionmgr->IsUserOnline(user.username))
	{
		SendLoginFailed(conn, "already logged in elsewhere");
		return;
	}

	m_sessionmgr->BindUser(fd, user.username, user.nickname);
	SendLoginSuccess(conn, user.username, user.nickname, true);
}

void Handler::RegisterHandle(Connection* conn, const std::string& body)
{
	auto j = json::parse(body);
	int username = j[message_schema::USERNAME].get<int>();
	std::string nickname = j[message_schema::NICKNAME].get<std::string>();
	std::string password = j[message_schema::PASSWORD].get<std::string>();

	MysqlClient* mysql = m_mysqlpool->Acquire();
	UserDao dao(mysql);
	if (dao.ExistsByUsername(username))
	{
		m_mysqlpool->Release(mysql);
		SendRegisterFailed(conn, "username already exists");
		return;
	}
	bool ok = dao.InsertUser(username, nickname, password);
	m_mysqlpool->Release(mysql);
	if (!ok)
	{
		SendRegisterFailed(conn, "database error");
		return;
	}
	SendRegisterSuccess(conn);
}

void Handler::ChatHandle(Connection* conn, const std::string& body)
{
	int fd = conn->fd;
	std::string nickname;
	if (!m_sessionmgr->GetNickName(fd, nickname))
	{
		perror("chat from unbound fd");
		return;
	}
	auto j = json::parse(body);
	std::string content = j[message_schema::CONTENT].get<std::string>();
	json msg;
	msg[message_schema::SENDER_NICKNAME] = nickname;
	msg[message_schema::CONTENT] = content;
	msg[message_schema::TIMESTAMP] = std::time(nullptr);
	std::string bodyout = msg.dump();
	std::string packet = Protocol::Pack(protocol::MessageType::CHAT_RECV, bodyout);
	
	std::vector<int> fds = m_sessionmgr->GetAllFds();
	for (auto& target_fd : fds)
	{
		SendTask task{ target_fd,packet };
		m_sendqueue->push(std::move(task));
	}
}

void Handler::PrivateChatHandle(Connection* conn, const std::string& body)
{
	int fd = conn->fd;
	std::string nickname;
	if (!m_sessionmgr->GetNickName(fd, nickname))
	{
		perror("chat from unbound fd");
		return;
	}
	auto j = json::parse(body);
	std::string content = j[message_schema::CONTENT].get<std::string>();
	int targetusername = j[message_schema::TARGET_USERNAME].get<int>();
	int targetfd = -1;
	if (!m_sessionmgr->GetFdByUsername(targetusername, targetfd))
	{
		std::cout << "未找到私聊对象:" << targetusername << std::endl;
		return;
	}
	json msg;
	msg[message_schema::SENDER_NICKNAME] = nickname;
	msg[message_schema::CONTENT] = content;
	msg[message_schema::TIMESTAMP] = std::time(nullptr);
	std::string bodyout = msg.dump();
	std::string packet = Protocol::Pack(protocol::MessageType::PRIVATE_CHAT_RECV, bodyout);
	SendTask task{ targetfd,packet };
	m_sendqueue->push(task);
}

void Handler::UserListHandle(Connection* conn, const std::string& body)
{
	int fd = conn->fd;
	auto users = m_sessionmgr->GetAllOnlineUsers();
	json arr = json::array();
	for (const auto& u : users)
	{
		json j;
		j[message_schema::USERNAME] = u.username;
		j[message_schema::NICKNAME] = u.nickname;
		j[message_schema::ONLINE] = u.online;
		arr.push_back(j);
	}
	json msg;
	msg[message_schema::USERS] = arr;
	std::string bodyout = msg.dump();
	std::string packet = Protocol::Pack(protocol::MessageType::USER_LIST_RESPONSE, bodyout);
	SendTask task{ fd,packet };
	m_sendqueue->push(task);
	std::cout << "发送在线用户列表" << std::endl;
}

void Handler::LogoutHandle(Connection* conn, const std::string& body)
{
	int fd = conn->fd;
	int username = -1;
	std::string nickname;
	if (m_sessionmgr->UnBindFd(fd, username, nickname))
	{
		std::cout << "logout " << username << ' ' << nickname << std::endl;
	}
	json j;
	std::string bodyout = j.dump();
	std::string packet = Protocol::Pack(protocol::MessageType::LOGOUT_SUCCESS, bodyout);
	SendTask task{ fd,packet };
	m_sendqueue->push(task);

	json j02;
	j02[message_schema::USERNAME] = username;
	j02[message_schema::NICKNAME] = nickname;
	std::string body02 = j02.dump();
	std::string packet02 = Protocol::Pack(protocol::MessageType::USER_LEAVE, body02);

	std::vector<int> fds = m_sessionmgr->GetAllFds();
	for (auto& target_fd : fds)
	{
		SendTask task02{ target_fd,packet02 };
		m_sendqueue->push(std::move(task02));
	}
}

void Handler::SendLoginSuccess(Connection* conn, int username, const std::string& nickname, bool online)
{
	json j;
	j[message_schema::USERNAME] = username;
	j[message_schema::NICKNAME] = nickname;
	j[message_schema::ONLINE] = online;
	std::string body = j.dump();
	std::string packet = Protocol::Pack(protocol::MessageType::LOGIN_SUCCESS, body);
	SendTask task{ conn->fd,packet };
	m_sendqueue->push(task);
	json j02;
	j02[message_schema::USERNAME] = username;
	j02[message_schema::NICKNAME] = nickname;
	j02[message_schema::ONLINE] = online;
	std::string body02 = j02.dump();
	std::string packet02 = Protocol::Pack(protocol::MessageType::USER_JOIN, body02);

	std::vector<int> fds = m_sessionmgr->GetAllFds();
	for (auto& target_fd : fds)
	{
		SendTask task02{ target_fd,packet02 };
		m_sendqueue->push(std::move(task02));
	}
}

void Handler::SendLoginFailed(Connection* conn, const std::string& reason)
{
	json j;
	j[message_schema::REASON] = reason;
	std::string body = j.dump();
	std::string packet = Protocol::Pack(protocol::MessageType::LOGIN_FAILED, body);
	SendTask task{ conn->fd,packet };
	m_sendqueue->push(task);
}

void Handler::SendRegisterSuccess(Connection* conn)
{
	json j;
	bool b = true;
	j[message_schema::SUCCESS] = b;
	std::string body = j.dump();
	std::string packet = Protocol::Pack(protocol::MessageType::REGISTER_SUCCESS, body);
	SendTask task{ conn->fd,packet };
	m_sendqueue->push(task);
}

void Handler::SendRegisterFailed(Connection* conn, const std::string& reason)
{
	json j;
	j[message_schema::REASON] = reason;
	std::string body = j.dump();
	std::string packet = Protocol::Pack(protocol::MessageType::REGISTER_FAILED, body);
	SendTask task{ conn->fd,packet };
	m_sendqueue->push(task);
}