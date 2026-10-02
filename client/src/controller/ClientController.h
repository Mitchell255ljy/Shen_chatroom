#pragma once

#include <QObject>
#include <QString>
#include <QByteArray>
#include <QList>

#include "model/User.h"
#include "model/ChatMessage.h"
#include "network/ConfigManager.h"
class Tcp;

class ClientController : public QObject
{
    Q_OBJECT

public:

    ClientController();
    ~ClientController();

    void connectToServer(const QString& host,quint16 port);

    void login(const int& username,const QString& password);
    void registerUser(const int& username,const QString& nickname,const QString& password);
    void logout();

    void sendChatMessage(const QString& content);
    void sendPrivateChat(int targetusername,const QString& content);

    void requestUserList();

    User currentUser() const;

    QList<User> onlineUsers() const;


signals:

    void connected();
    void disconnected();
    void networkError(const QString& error);//slot:Login.h Chat.h

    void loginSucceeded();//slot:Login::onLoginSucceeded  WindowManager::onLoginSucceeded
    void loginFailed(const QString& reason);//slot:Login::onLoginFailed

    void registerSucceeded();//slot:Register::onRegisterSucceeded
    void registerFailed(const QString& reason);//slot:Register::onRegisterFailed

    void chatMessageReceived(const ChatMessage& message);//slot:ClientController::chatMessageReceived
    void privateChatMessageReceived(const ChatMessage& message);

    void userJoined(const User& user);//slot:Chat::onUserJoined
    void userLeft(const User& user);//slot:Chat::onUserLeft

    void userListUpdated();//slot:Chat::onUserListUpdated

    void loggedOut();//slot:WindowManager::onLoggedOut
    void logoutFailed(const QString& reason);


private slots:

    void onConnected();//sig:Tcp::connected
    void onDisconnected();//sig:Tcp::disconnected

    void onDataReceived(const QByteArray& data);//sig:Tcp::dataReceived

    void onConnectionError(const QString& error);//sig:Tcp::connectionError


private:

    void processServerData();

    void handleLoginSuccess(const QByteArray& body);
    void handleLoginFailed( const QByteArray& body);
    void handleRegisterSuccess(const QByteArray& body);
    void handleRegisterFailed(const QByteArray& body);
    void handleChatMessage(const QByteArray& body);
    void handlePrivateChatMessage(const QByteArray& body);
    void handleUserJoin(const QByteArray& body);
    void handleUserLeave(const QByteArray& body);
    void handleUserList(const QByteArray& body);
    void handleLogoutSuccess(const QByteArray& body);
    void handleLogoutFailed(const QByteArray& body);
private:

    Tcp* m_tcp;
    QByteArray m_buffer;
    User m_currentUser;
    QList<User> m_onlineUsers;

    QString host;
    quint16 port;
};