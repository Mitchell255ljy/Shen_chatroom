#include "ClientController.h"

#include "network/Tcp.h"
#include "network/Protocol.h"

#include "../../../common/message_schema.h"

#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include<QDebug>

ClientController::ClientController() : m_tcp(nullptr),m_buffer(),m_currentUser(),m_onlineUsers()
{
    m_tcp = new Tcp();

    connect(m_tcp,&Tcp::connected,this,&ClientController::onConnected);
    connect(m_tcp,&Tcp::disconnected,this,&ClientController::onDisconnected);
    connect(m_tcp,&Tcp::dataReceived,this,&ClientController::onDataReceived);
    connect(m_tcp,&Tcp::connectionError,this,&ClientController::onConnectionError);
}


ClientController::~ClientController()
{
    delete m_tcp;
}

void ClientController::connectToServer(const QString& host,quint16 port)
{
    m_tcp->connectToServer(host,port);
}

void ClientController::login(const int& username,const QString& password)
{
    if(!(m_tcp->isConnected()))
    {
        std::unordered_map<std::string,std::string> configmap=ConfigManager::readServerConfig();
        host=QString::fromStdString(configmap["HOST"]);
        port=QString::fromStdString(configmap["PORT"]).toInt();
        connectToServer(host,port);
    }
    QByteArray data=Protocol::packLoginRequest(username,password);
    m_tcp->sendData(data);
}

void ClientController::registerUser(const int& username,const QString& nickname,const QString& password)
{
    if(!(m_tcp->isConnected()))
    {
        std::unordered_map<std::string,std::string> configmap=ConfigManager::readServerConfig();
        host=QString::fromStdString(configmap["HOST"]);
        port=QString::fromStdString(configmap["PORT"]).toInt();
        connectToServer(host,port);
    }
    QByteArray data=Protocol::packRegisterRequest(username,nickname,password);
    m_tcp->sendData(data);
}

void ClientController::logout()
{
    if(!(m_tcp->isConnected()))
    {
        std::unordered_map<std::string,std::string> configmap=ConfigManager::readServerConfig();
        host=QString::fromStdString(configmap["HOST"]);
        port=QString::fromStdString(configmap["PORT"]).toInt();
        connectToServer(host,port);
    }
    QByteArray data=Protocol::packLogoutRequest();
    m_tcp->sendData(data);
}

void ClientController::sendChatMessage(const QString& content)
{
    if(!(m_tcp->isConnected()))
    {
        std::unordered_map<std::string,std::string> configmap=ConfigManager::readServerConfig();
        host=QString::fromStdString(configmap["HOST"]);
        port=QString::fromStdString(configmap["PORT"]).toInt();
        connectToServer(host,port);
    }
    QByteArray data=Protocol::packChatMessage(content);
    m_tcp->sendData(data);
}

void ClientController::sendPrivateChat(int targetusername,const QString& content)
{
    if(!(m_tcp->isConnected()))
    {
        std::unordered_map<std::string,std::string> configmap=ConfigManager::readServerConfig();
        host=QString::fromStdString(configmap["HOST"]);
        port=QString::fromStdString(configmap["PORT"]).toInt();
        connectToServer(host,port);
    }
    QByteArray data=Protocol::packPrivateChatMessage(targetusername,content);
    m_tcp->sendData(data);
}

void ClientController::requestUserList()
{
    if(!(m_tcp->isConnected()))
    {
        std::unordered_map<std::string,std::string> configmap=ConfigManager::readServerConfig();
        host=QString::fromStdString(configmap["HOST"]);
        port=QString::fromStdString(configmap["PORT"]).toInt();
        connectToServer(host,port);
    }
    QByteArray data=Protocol::packUserListRequest();
    qDebug()<<"接收到的用户列表:"<<data;
    m_tcp->sendData(data);
}

User ClientController::currentUser() const
{
    return m_currentUser;
}

QList<User> ClientController::onlineUsers() const
{
    return m_onlineUsers;
}

void ClientController::onConnected()
{
    emit connected();
}

void ClientController::onDisconnected()
{

    m_buffer.clear();
    m_onlineUsers.clear();
    m_currentUser=User();
    emit disconnected();
}

void ClientController::onDataReceived(const QByteArray& data)
{
    m_buffer.append(data);
    processServerData();
}

void ClientController::onConnectionError(
    const QString& error)
{
    emit networkError(error);
}

void ClientController::processServerData()
{
    protocol::MessageType type;
    QByteArray body;
    while (Protocol::parsePacket(m_buffer, type, body))
    {
        switch (type)
        {
        case protocol::MessageType::LOGIN_SUCCESS:
            handleLoginSuccess(body);
            break;

        case protocol::MessageType::LOGIN_FAILED:
            handleLoginFailed(body);
            break;

        case protocol::MessageType::REGISTER_SUCCESS:
            handleRegisterSuccess(body);
            break;

        case protocol::MessageType::REGISTER_FAILED:
            handleRegisterFailed(body);
            break;

        case protocol::MessageType::CHAT_RECV:
            handleChatMessage(body);
            break;

        case protocol::MessageType::PRIVATE_CHAT_RECV:
            handlePrivateChatMessage(body);
            break;

        case protocol::MessageType::USER_JOIN:
            handleUserJoin(body);
            break;

        case protocol::MessageType::USER_LEAVE:
            handleUserLeave(body);
            break;

        case protocol::MessageType::USER_LIST_RESPONSE:
            handleUserList(body);
            break;

        case protocol::MessageType::LOGOUT_SUCCESS:
            handleLogoutSuccess(body);
            break;

        case protocol::MessageType::LOGOUT_FAILED:
            handleLogoutFailed(body);
            break;

        default:
            qWarning() << "processServerData: Unknown message type:" << static_cast<int>(type);
            break;
        }
    }
}

void ClientController::handleLoginSuccess(const QByteArray& body)
{
    QJsonDocument jsonDoc = QJsonDocument::fromJson(body);
    QJsonObject jsonObj = jsonDoc.object();

    m_currentUser.setUsername(jsonObj[message_schema::USERNAME].toInt());
    m_currentUser.setNickname(jsonObj[message_schema::NICKNAME].toString());
    m_currentUser.setOnline(jsonObj[message_schema::ONLINE].toBool(true));

    m_onlineUsers.clear();

    emit loginSucceeded();
}

void ClientController::handleLoginFailed(const QByteArray& body)
{
    QJsonDocument jsonDoc = QJsonDocument::fromJson(body);
    QJsonObject jsonObj=jsonDoc.object();
    QString reason = jsonObj[message_schema::REASON].toString();
    emit loginFailed(reason);
}

void ClientController::handleRegisterSuccess(const QByteArray& body)
{
    emit registerSucceeded();
    Q_UNUSED(body);
}

void ClientController::handleRegisterFailed(const QByteArray& body)
{
    QJsonDocument jsonDoc = QJsonDocument::fromJson(body);
    QJsonObject jsonObj = jsonDoc.object();
    QString reason = jsonObj[message_schema::REASON].toString();
    emit registerFailed(reason);
}

void ClientController::handleChatMessage(const QByteArray& body)
{
    QJsonDocument jsonDoc = QJsonDocument::fromJson(body);
    QJsonObject jsonObj = jsonDoc.object();

    ChatMessage message;
    message.setSenderNickname(jsonObj[message_schema::SENDER_NICKNAME].toString());
    message.setContent(jsonObj[message_schema::CONTENT].toString());
    message.setTimestamp(QDateTime::currentDateTime());
    message.setType(ChatMessage::Type::Public);

    emit chatMessageReceived(message);
}

void ClientController::handlePrivateChatMessage(const QByteArray& body)
{
    QJsonDocument jsonDoc = QJsonDocument::fromJson(body);
    QJsonObject jsonObj = jsonDoc.object();

    ChatMessage message;
    message.setSenderNickname(jsonObj[message_schema::SENDER_NICKNAME].toString());
    message.setContent(jsonObj[message_schema::CONTENT].toString());
    message.setTimestamp(QDateTime::currentDateTime());
    message.setType(ChatMessage::Type::Private);
    message.setReceiverNickname(m_currentUser.nickname());

    emit privateChatMessageReceived(message);
}

void ClientController::handleUserJoin(const QByteArray& body)
{
    QJsonDocument jsonDoc = QJsonDocument::fromJson(body);
    QJsonObject jsonObj = jsonDoc.object();
    int username=jsonObj[message_schema::USERNAME].toInt();
    QString nickname = jsonObj[message_schema::NICKNAME].toString();
    bool online = jsonObj[message_schema::ONLINE].toBool(true);

    User newUser(username, nickname, online);

    bool exists = false;
    for (int i = 0; i < m_onlineUsers.size(); ++i)
    {
        if (m_onlineUsers[i].username() == username)
        {
            m_onlineUsers[i] = newUser;
            exists = true;
            break;
        }
    }

    if (!exists)
    {
        m_onlineUsers.append(newUser);
    }

    emit userJoined(newUser);
    emit userListUpdated();
}

void ClientController::handleUserLeave(const QByteArray& body)
{
    QJsonDocument jsonDoc = QJsonDocument::fromJson(body);
    QJsonObject jsonObj = jsonDoc.object();

    int username = jsonObj[message_schema::USERNAME].toInt();
    QString nickname = jsonObj[message_schema::NICKNAME].toString();

    User leftUser;
    bool found = false;

    qDebug() << "[USER_LEAVE] recv username =" << username
             << "nickname =" << nickname;
    qDebug() << "[USER_LEAVE] current m_onlineUsers:";
    for (const User& u : m_onlineUsers)
        qDebug() << "   user:" << u.username() << u.nickname();

    for (int i = 0; i < m_onlineUsers.size(); ++i)
    {
        if (m_onlineUsers[i].username() == username)
        {
            leftUser = m_onlineUsers[i];
            m_onlineUsers.removeAt(i);
            found = true;
            break;
        }
    }

    qDebug() << "[USER_LEAVE] found =" << found
             << "m_onlineUsers.size() after =" << m_onlineUsers.size();

    if (!found)
    {
        leftUser = User(username, nickname, false);
    }

    emit userLeft(leftUser);
    emit userListUpdated();
}

void ClientController::handleUserList(
    const QByteArray& body)
{
    QJsonDocument jsonDoc = QJsonDocument::fromJson(body);
    QJsonObject jsonObj = jsonDoc.object();
    QJsonArray usersArray = jsonObj[message_schema::USERS].toArray();

    m_onlineUsers.clear();

    for (const auto& userValue : usersArray)
    {
        QJsonObject userObj = userValue.toObject();
        int username = userObj[message_schema::USERNAME].toInt();
        QString nickname = userObj[message_schema::NICKNAME].toString();
        bool online = userObj[message_schema::ONLINE].toBool(true);

        if (username>0&&!nickname.isEmpty())
        {
            m_onlineUsers.append(User(username, nickname, online));
        }
    }

    emit userListUpdated();
}

void ClientController::handleLogoutSuccess(
    const QByteArray& body)
{
    m_currentUser = User();
    m_onlineUsers.clear();
    m_buffer.clear();

    emit loggedOut();
    if(m_tcp->isConnected())
    {
        m_tcp->disconnectFromServer();
    }
}

void ClientController::handleLogoutFailed(const QByteArray& body)
{

    QJsonDocument jsonDoc = QJsonDocument::fromJson(body);
    QJsonObject jsonObj = jsonDoc.object();
    QString reason = jsonObj[message_schema::REASON].toString();
    emit logoutFailed(reason);
    Q_UNUSED(body);
}