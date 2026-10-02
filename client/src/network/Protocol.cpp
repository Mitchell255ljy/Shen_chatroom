#include "Protocol.h"

#include "../../../common/message_schema.h"

#include <QJsonDocument>
#include <QJsonObject>

#include <cstring>


namespace
{
    QByteArray buildPacket(protocol::MessageType type,const QByteArray& body)
    {
        protocol::Header header;
        header.magic = protocol::MAGIC;
        header.type = static_cast<std::uint8_t>(type);
        header.bodyLength = static_cast<std::uint32_t>(body.size());

        QByteArray packet;

        packet.append(reinterpret_cast<const char*>(&header),sizeof(header));

        packet.append(body);

        return packet;
    }
}

QByteArray Protocol::packLoginRequest(const int& username,const QString& password)
{
    QJsonObject jsonObject;
    jsonObject[message_schema::USERNAME] = username;
    jsonObject[message_schema::PASSWORD] = password;

    QJsonDocument jsonDoc(jsonObject);
    QByteArray jsonData = jsonDoc.toJson(QJsonDocument::Compact);

    return buildPacket(protocol::MessageType::LOGIN_REQUEST,jsonData);
}

QByteArray Protocol::packRegisterRequest(const int& username,const QString& nickname,const QString& password)
{
    QJsonObject jsonObject;
    jsonObject[message_schema::USERNAME] = username;
    jsonObject[message_schema::NICKNAME] = nickname;
    jsonObject[message_schema::PASSWORD] = password;

    QJsonDocument jsonDoc(jsonObject);
    QByteArray jsonData = jsonDoc.toJson(QJsonDocument::Compact);

    return buildPacket(protocol::MessageType::REGISTER_REQUEST,jsonData);
}

QByteArray Protocol::packChatMessage(const QString& content)
{
    QJsonObject jsonObject;
    jsonObject[message_schema::CONTENT] = content;

    QJsonDocument jsonDoc(jsonObject);
    QByteArray jsonData = jsonDoc.toJson(QJsonDocument::Compact);

    return buildPacket(protocol::MessageType::CHAT_SEND,jsonData);
}

QByteArray Protocol::packPrivateChatMessage(int targetusername,const QString& content)
{
    QJsonObject jsonObject;
    jsonObject[message_schema::TARGET_USERNAME]=targetusername;
    jsonObject[message_schema::CONTENT]=content;

    QJsonDocument jsonDoc(jsonObject);
    QByteArray jsonData=jsonDoc.toJson(QJsonDocument::Compact);

    return buildPacket(protocol::MessageType::PRIVATE_CHAT_SEND,jsonData);
}

QByteArray Protocol::packUserListRequest()
{
    QJsonObject jsonObject;
    QJsonDocument jsonDoc(jsonObject);
    QByteArray jsonData = jsonDoc.toJson(QJsonDocument::Compact);

    return buildPacket(protocol::MessageType::USER_LIST_REQUEST,jsonData);
    return QByteArray();
}

QByteArray Protocol::packLogoutRequest()
{
    QJsonObject jsonObject;
    QJsonDocument jsonDoc(jsonObject);
    QByteArray jsonData = jsonDoc.toJson(QJsonDocument::Compact);

    return buildPacket(protocol::MessageType::LOGOUT_REQUEST,jsonData);
    return QByteArray();
}

bool Protocol::hasCompletePacket(const QByteArray& buffer)
{
    if (buffer.size() < static_cast<int>(protocol::HEADER_SIZE))
    {
        return false;
    }

    protocol::Header header;
    memcpy(&header, buffer.constData(), sizeof(header));

    if (header.magic != protocol::MAGIC)
    {
        qWarning() << "Protocol: Invalid magic number" << header.magic;
        return false;
    }

    if (header.bodyLength > protocol::MAX_BODY_LENGTH)
    {
        qWarning() << "Protocol: Body length too large" << header.bodyLength;
        return false;
    }

    std::size_t totalPacketSize = protocol::HEADER_SIZE + header.bodyLength;
    if (buffer.size() >= static_cast<int>(totalPacketSize))
    {
        return true;
    }

    return false;
}

bool Protocol::parsePacket(QByteArray& buffer,protocol::MessageType& type,QByteArray& body)
{
    if (buffer.isEmpty())
    {
        return false;
    }

    if (buffer.size() < static_cast<int>(protocol::HEADER_SIZE))
    {
        return false;
    }

    protocol::Header header;
    memcpy(&header, buffer.constData(), sizeof(header));

    if (header.magic != protocol::MAGIC)
    {
        qWarning() << "Protocol: Invalid magic number" << header.magic;
        buffer.clear();
        return false;
    }

    if (header.bodyLength > protocol::MAX_BODY_LENGTH)
    {
        qWarning() << "Protocol: Body length too large" << header.bodyLength;
        buffer.clear();
        return false;
    }

    std::size_t totalPacketSize = protocol::HEADER_SIZE + header.bodyLength;
    if (buffer.size() < static_cast<int>(totalPacketSize))
    {
        return false;
    }

    std::uint8_t typeValue = header.type;
    type = static_cast<protocol::MessageType>(typeValue);

    if (header.bodyLength > 0) {
        body = buffer.mid(protocol::HEADER_SIZE, header.bodyLength);
    } else {
        body.clear();
    }

    buffer.remove(0, totalPacketSize);

    return true;
}