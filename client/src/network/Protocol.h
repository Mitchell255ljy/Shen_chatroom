#pragma once

#include <QByteArray>
#include <QString>

#include "../../../common/protocol.h"
class Protocol
{
public:

    Protocol() = default;

    static QByteArray packLoginRequest(const int& username,const QString& password);

    static QByteArray packRegisterRequest(const int& username,const QString& nickname,const QString& password);

    static QByteArray packChatMessage(const QString& content);

    static QByteArray packPrivateChatMessage(int targetusername,const QString& content);

    static QByteArray packUserListRequest();

    static QByteArray packLogoutRequest();

    static bool hasCompletePacket(const QByteArray& buffer);

    static bool parsePacket(QByteArray& buffer,protocol::MessageType& type,QByteArray& body);
};