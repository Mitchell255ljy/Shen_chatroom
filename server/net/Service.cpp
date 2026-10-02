#include"Service.h"
Service::Service(int thread_num,int mysql_num) :mypool(thread_num),mysqlpool(mysql_num), m_handler(&sessionmgr, &mysqlpool, &send_queue)
{
	std::unordered_map<std::string, std::string> configmap = ConfigManager::ReadServerConfig();
	uint16_t SER_PORT = std::stoi(configmap["PORT"]);
	std::string SER_IP = configmap["HOST"];

	m_sfd = socket(AF_INET, SOCK_STREAM, 0);
	if (m_sfd == -1)
	{
		perror("socket error");
		exit(1);
	}
	int opt = 1;
	setsockopt(m_sfd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
	struct sockaddr_in sin;
	sin.sin_family = AF_INET;
	sin.sin_port = htons(SER_PORT);
	sin.sin_addr.s_addr = inet_addr(SER_IP.c_str());
	if (bind(m_sfd, (struct sockaddr*)&sin, sizeof(sin)) == -1)
	{
		perror("bind error");
		exit(1);
	}
	if (listen(m_sfd, 128) == -1)
	{
		perror("listen error");
		exit(1);
	}

	m_epfd = epoll_create(1);
	if (m_epfd == -1)
	{
		perror("epoll_create error");
		exit(1);
	}
	struct epoll_event ev;
	ev.events = EPOLLIN;
	ev.data.fd = m_sfd;
	epoll_ctl(m_epfd, EPOLL_CTL_ADD, m_sfd, &ev);
	
	m_wakefd = eventfd(0, EFD_NONBLOCK | EFD_CLOEXEC);
	send_queue.SetWakeFd(m_wakefd);
	if (m_wakefd == -1)
	{
		perror("eventfd error");
		exit(1);
	}
	ev.events = EPOLLIN;
	ev.data.fd = m_wakefd;
	epoll_ctl(m_epfd, EPOLL_CTL_ADD, m_wakefd, &ev);
}

Service::~Service()
{
	running = false;
	if (m_wakefd != -1)
	{
		uint64_t one = 1;
		ssize_t r = write(m_wakefd, &one, sizeof(one));
		(void)r;
	}
	if (m_sfd != -1)
		close(m_sfd);
	if (m_epfd != -1)
		close(m_epfd);
	if (m_wakefd != -1)
		close(m_wakefd);
}

void Service::Run()
{
	epoll_event evs[1024];
	int evs_size = sizeof(evs) / sizeof(evs[0]);
	while (running)
	{
		int num = epoll_wait(m_epfd, evs, evs_size, -1);
		if (num == -1)
		{
			perror("epoll_wait error");
			exit(1);
		}
		for (int i = 0; i < num; i++)
		{
			int fd = evs[i].data.fd;
			uint32_t ev = evs[i].events;
			if (fd == m_sfd)
			{
				HandleAccept();
			}
			else if (fd == m_wakefd)
			{
				HandleWake();
			}
			else
			{
				if (ev & (EPOLLERR | EPOLLHUP))
				{
					CloseConn(fd, "epoll error/hup");
					continue;
				}
				if (ev & EPOLLIN)
				{
					HandleRead(fd);
				}
				if (ev & EPOLLOUT)
				{
					HandleWrite(fd);
				}
			}
		}
		FlushSendQueue();
	}
}

void Service::HandleAccept()
{
	struct sockaddr_in cin;
	socklen_t cinlen = sizeof(cin);
	int newfd = accept(m_sfd, (struct sockaddr*)&cin, &cinlen);
	if (newfd == -1)
	{
		perror("accept error");
		return;
	}
	printf("[%s:%d]:已连接成功，newfd = %d\n",
		inet_ntoa(cin.sin_addr), ntohs(cin.sin_port), newfd);

	conns.GetOrCreate(newfd);
	struct epoll_event newev;
	newev.events = EPOLLIN;
	newev.data.fd = newfd;
	epoll_ctl(m_epfd, EPOLL_CTL_ADD, newfd, &newev);
}

void Service::HandleRead(int fd)
{
	Connection* conn = conns.find(fd);
	if (!conn)
	{
		CloseConn(fd, "no connection");
		return;
	}
	ssize_t n = recv(fd, m_buffer, sizeof(m_buffer), 0);
	if (n == 0)
	{
		CloseConn(fd, "对端下线");
		return;
	}
	else if (n < 0)
	{
		if (errno == EAGAIN || errno == EWOULDBLOCK)return;
		if (errno == EINTR)return;
		perror("recv error");
		CloseConn(fd, "recv error");
		return;
	}
	conn->AppendRead(m_buffer, n);
	ProcessClientData(conn);
}

void Service::HandleWrite(int fd)
{
	Connection* conn = conns.find(fd);
	if (!conn)return;
	TrySend(fd);
}

void Service::CloseConn(int fd, const char* reason)
{
	printf("关闭fd=%d,原因: %s\n", fd, reason);

	int username = -1;
	std::string nickname;
	bool wasBound = sessionmgr.UnBindFd(fd, username, nickname);

	if (wasBound)
	{
		std::cout << "[CloseConn] auto-unbind user "<< username << " " << nickname<< " (fd=" << fd << ")" << std::endl;

		json j;
		j[message_schema::USERNAME] = username;
		j[message_schema::NICKNAME] = nickname;
		std::string body = j.dump();
		std::string packet = Protocol::Pack(protocol::MessageType::USER_LEAVE, body);

		std::vector<int> fds = sessionmgr.GetAllFds();
		for (auto& target_fd : fds)
		{
			if (target_fd == fd) continue;
			SendTask task{ target_fd, packet };
			send_queue.push(std::move(task));
		}
	}

	epoll_ctl(m_epfd, EPOLL_CTL_DEL, fd, nullptr);
	close(fd);
	conns.erase(fd);

}

void Service::ProcessClientData(Connection* conn)
{
	protocol::MessageType type;
	std::string body;
	while (Protocol::ParsePacket(conn->read_buf, type, body))
	{
		switch (type)
		{
		case protocol::MessageType::LOGIN_REQUEST:
			mypool.AddTasks([this, conn, body]() {m_handler.LoginHandle(conn, body); });
			break;
		case protocol::MessageType::REGISTER_REQUEST:
			mypool.AddTasks([this, conn, body]() {m_handler.RegisterHandle(conn, body); });
			break;
		case protocol::MessageType::CHAT_SEND:
			mypool.AddTasks([this, conn, body]() {m_handler.ChatHandle(conn, body); });
			break;
		case protocol::MessageType::PRIVATE_CHAT_SEND:
			mypool.AddTasks([this, conn, body]() {m_handler.PrivateChatHandle(conn, body); });
			break;
		case protocol::MessageType::USER_LIST_REQUEST:
			mypool.AddTasks([this, conn, body]() {m_handler.UserListHandle(conn, body); });
			break;
		case protocol::MessageType::LOGOUT_REQUEST:
			mypool.AddTasks([this, conn, body]() {m_handler.LogoutHandle(conn, body); });
			break;
		default:
			perror("message_type error");
			break;
		}
	}
}

void Service::HandleWake()
{
	uint64_t val = 0;
	ssize_t r = read(m_wakefd, &val, sizeof(val));
	(void)r;
}

void Service::FlushSendQueue()
{
	auto tasks = send_queue.drain();
	for (auto& task : tasks)
	{
		int fd = task.fd;
		Connection* conn = conns.find(fd);
		if (!conn)
		{
			continue;
		}
		conn->AppendWrite(task.data.data(),task.data.size());
		TrySend(fd);
	}
}

void Service::TrySend(int fd)
{
	Connection* conn = conns.find(fd);
	if (!conn)return;
	while (!conn->write_buf.empty())
	{
		ssize_t n = send(fd, conn->write_buf.data(), conn->write_buf.size(), MSG_NOSIGNAL);
		if (n > 0)
		{
			conn->write_buf.erase(0, n);
		}
		else if (n == -1)
		{
			if (errno == EAGAIN || errno == EWOULDBLOCK)
			{
				EnableWrite(fd, true);
				return;
			}
			if (errno == EINTR)
			{
				continue;
			}
			perror("send error");
			CloseConn(fd, "send error");
			return;
		}
		else
		{
			break;
		}
	}
	if (conn->write_buf.empty())
	{
		EnableWrite(fd, false);
	}
}

void Service::EnableWrite(int fd, bool on)
{
	Connection* conn = conns.find(fd);
	if (!conn)return;
	if (conn->want_write == on)return;
	struct epoll_event ev;
	ev.events = EPOLLIN | (on ? EPOLLOUT : 0);
	ev.data.fd = fd;
	if (epoll_ctl(m_epfd, EPOLL_CTL_MOD, fd, &ev) == -1)
	{
		perror("epoll_ctl MOD error");
		return;
	}
	conn->want_write = on;
}