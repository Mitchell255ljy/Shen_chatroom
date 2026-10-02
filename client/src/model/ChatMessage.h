#pragma once

#include <QString>
#include <QDateTime>

class ChatMessage
{
public:

    enum class Type
    {
        Public = 0,
        Private
    };

    ChatMessage();
    ChatMessage(const QString& senderNickname,const QString& content);

    QString senderNickname() const;
    QString content() const;
    QDateTime timestamp() const;
    Type type() const;
    QString receiverNickname() const;

    void setSenderNickname(const QString& senderNickname);
    void setContent(const QString& content);
    void setTimestamp(const QDateTime& timestamp);
    void setType(Type type);
    void setReceiverNickname(const QString& receiverNickname);


private:

    QString m_senderNickname;
    QString m_content;
    QDateTime m_timestamp;
    Type m_type;
    QString m_receiverNickname;
};