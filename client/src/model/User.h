#pragma once

#include <QString>

class User
{
public:

    User();
    User(int username,const QString& nickname,bool online = false);

    int username() const;
    QString nickname() const;
    bool isOnline() const;

    void setUsername(int username);
    void setNickname(const QString& nickname);
    void setOnline(bool online);
private:

    int m_username;
    QString m_nickname;
    bool m_online;
};