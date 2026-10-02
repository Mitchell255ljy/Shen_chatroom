#include "User.h"

User::User(): m_username(-1),m_nickname(),m_online(false){}

User::User(int username,const QString& nickname,bool online) : m_username(username),m_nickname(nickname),m_online(online){}

int User::username() const
{
    return m_username;
}

QString User::nickname() const
{
    return m_nickname;
}

bool User::isOnline() const
{
    return m_online;
}

void User::setUsername(int username)
{
    m_username=username;
}

void User::setNickname(const QString& nickname)
{
    m_nickname = nickname;
}

void User::setOnline(bool online)
{
    m_online = online;
}