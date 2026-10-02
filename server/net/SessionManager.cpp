#include"SessionManager.h"
SessionManager::SessionManager() = default;
SessionManager::~SessionManager() = default;

void SessionManager::BindUser(int fd, int username, std::string nickname)
{
	{
		std::lock_guard<std::mutex> lock(m_mutex);
		m_fdToSession[fd] = Session{ fd,username,nickname };
		m_usernameToFd[username] = fd;
		m_fds.insert(fd);
	}
}

bool SessionManager::UnBindFd(int fd, int& username, std::string& nickname)
{
	{
		std::lock_guard<std::mutex> lock(m_mutex);
		auto it = m_fdToSession.find(fd);
		if (it == m_fdToSession.end())
			return false;
		username = it->second.username;
		nickname = it->second.nickname;
		m_fdToSession.erase(it);
		m_usernameToFd.erase(username);
		m_fds.erase(fd);
		return true;
	}
}

bool SessionManager::GetUserName(int fd, int& out_username) const
{
	{
		std::lock_guard<std::mutex> lock(m_mutex);
		auto it = m_fdToSession.find(fd);
		if (it == m_fdToSession.end())
			return false;
		out_username = it->second.username;
		return true;
	}
}

bool SessionManager::GetNickName(int fd, std::string& out_nickname) const
{
	{
		std::lock_guard<std::mutex> lock(m_mutex);
		auto it = m_fdToSession.find(fd);
		if (it == m_fdToSession.end())
			return false;
		out_nickname = it->second.nickname;
		return true;
	}
}

bool SessionManager::GetFdByUsername(int username, int& out_fd) const
{
	{
		std::lock_guard<std::mutex> lock(m_mutex);
		auto it = m_usernameToFd.find(username);
		if (it == m_usernameToFd.end())return false;
		out_fd = it->second;
		return true;
	}
}

std::vector<OnlineUser> SessionManager::GetAllOnlineUsers() const
{
	std::vector<OnlineUser> result;
	{
		std::lock_guard<std::mutex> lock(m_mutex);
		for (const auto& kv : m_fdToSession)
		{
			result.push_back(OnlineUser{kv.second.username,kv.second.nickname,true});
		}
		return result;
	}
}

bool SessionManager::IsUserOnline(int username) const
{
	std::lock_guard<std::mutex> lock(m_mutex);
	return m_usernameToFd.count(username) > 0;
}

std::vector<int> SessionManager::GetAllFds() const
{
	std::lock_guard<std::mutex> lock(m_mutex);
	return std::vector<int>(m_fds.begin(), m_fds.end());
}