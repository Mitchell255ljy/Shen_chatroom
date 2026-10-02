#pragma once

#include"ThreadPool.h"
#include"Connection.h"
#include"SendQueue.h"
#include"../util/ConfigManager.h"
#include"../../common/message_schema.h"
#include"../../common/protocol.h"
#include"../protocol/Protocol.h"
#include"../handler/Handler.h"
#include"../db/MysqlPool.h"
#include"SessionManager.h"

#include<iostream>
#include<sys/types.h>
#include<sys/socket.h>
#include<cstdio>
#include<cstdlib>
#include<unistd.h>
#include<sys/epoll.h>
#include<pthread.h>
#include<sys/stat.h>
#include<atomic>
#include<netinet/in.h>
#include<arpa/inet.h>
#include<sys/eventfd.h>
#include<errno.h>
#include<unordered_map>


class Service
{
public:
	Service(int thread_num,int mysql_num);
	~Service();
	void Run();

private:
	void HandleAccept();
	void HandleRead(int fd);
	void HandleWrite(int fd);
	void HandleWake();
	
	void FlushSendQueue();
	void TrySend(int fd);
	void EnableWrite(int fd, bool on);
	void CloseConn(int fd,const char* reason);
	
	void ProcessClientData(Connection* conn);

private:
	int m_sfd = -1;
	int m_epfd = -1;
	int m_wakefd = -1;
	ThreadPool mypool;
	SendQueue send_queue;
	ConnectionTable conns;
	MysqlPool mysqlpool;
	SessionManager sessionmgr;
	Handler m_handler;
	std::atomic<bool> running{ true };
	char m_buffer[4096];
};