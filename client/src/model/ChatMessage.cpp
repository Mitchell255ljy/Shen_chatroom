#include "ChatMessage.h"


ChatMessage::ChatMessage() : m_senderNickname(),m_content(),m_timestamp(QDateTime::currentDateTime()),m_type(Type::Public),m_receiverNickname(){}
ChatMessage::ChatMessage(const QString& senderNickname,const QString& content)
    : m_senderNickname(senderNickname),m_content(content),m_timestamp(QDateTime::currentDateTime()),m_type(Type::Public),m_receiverNickname(){}

QString ChatMessage::senderNickname() const
{
    return m_senderNickname;
}

QString ChatMessage::content() const
{
    return m_content;
}

QDateTime ChatMessage::timestamp() const
{
    return m_timestamp;
}

ChatMessage::Type ChatMessage::type() const
{
    return m_type;
}

QString ChatMessage::receiverNickname() const
{
    return m_receiverNickname;
}

void ChatMessage::setSenderNickname(const QString& senderNickname)
{
    m_senderNickname = senderNickname;
}

void ChatMessage::setContent(const QString& content)
{
    m_content = content;
}

void ChatMessage::setTimestamp(const QDateTime& timestamp)
{
    m_timestamp = timestamp;
}

void ChatMessage::setType(Type type)
{
    m_type = type;
}

void ChatMessage::setReceiverNickname(const QString& receiverNickname)
{
    m_receiverNickname = receiverNickname;
}