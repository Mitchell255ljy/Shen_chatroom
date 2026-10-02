#pragma once
#include<unordered_map>
#include<unordered_set>
#include<mutex>
#include<vector>
#include<string>
#include<cstdint>
#include<iostream>
#include<utility>

struct Session
{
	int fd;
	int username;
	std::string nickname;
};

struct OnlineUser
{
	int username;
	std::string nickname;
	bool online;
};

class SessionManager
{
public:
	SessionManager();
	~SessionManager();

	void BindUser(int fd, int username, std::string nickname);
	bool UnBindFd(int fd, int& username, std::string& nickname);

	bool GetUserName(int fd, int& out_username) const;
	bool GetNickName(int fd, std::string& out_nickname) const;
	bool GetFdByUsername(int username, int& out_fd) const;

	std::vector<OnlineUser> GetAllOnlineUsers() const;

	bool IsUserOnline(int username) const;

	std::vector<int> GetAllFds() const;

private:
	mutable std::mutex m_mutex;
	std::unordered_map<int, Session> m_fdToSession;
	std::unordered_map<int, int> m_usernameToFd;
	std::unordered_set<int> m_fds;
};