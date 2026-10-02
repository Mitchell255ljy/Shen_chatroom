#include "Tcp.h"

Tcp::Tcp()
{
    m_socket = new QTcpSocket(this);
    connect(m_socket,&QTcpSocket::connected,this,&Tcp::onConnected);
    connect(m_socket,&QTcpSocket::disconnected,this,&Tcp::onDisconnected);
    connect(m_socket,&QTcpSocket::readyRead,this,&Tcp::onReadyRead);
    connect(m_socket,&QTcpSocket::errorOccurred,this,&Tcp::onErrorOccurred);
}

Tcp::~Tcp()
{
}

void Tcp::connectToServer(const QString& host,quint16 port)
{
    m_socket->connectToHost(host,port);
}

void Tcp::disconnectFromServer()
{
    m_socket->disconnectFromHost();
}

bool Tcp::isConnected() const
{
    if(m_socket==nullptr)
        return false;
    return m_socket->state()==QAbstractSocket::ConnectedState;
}

void Tcp::sendData(const QByteArray& data)
{

    if(m_socket==nullptr||m_socket->state()!=QAbstractSocket::ConnectedState)
    {
        qDebug()<<"Tcp::sendData: socket not ready";
        return;
    }
    m_socket->write(data);
}

void Tcp::onConnected()
{
    emit connected();
}

void Tcp::onDisconnected()
{
    emit disconnected();
}

void Tcp::onReadyRead()
{
    QByteArray data = m_socket->readAll();
    emit dataReceived(data);
}

void Tcp::onErrorOccurred(
    QAbstractSocket::SocketError error)
{
    emit connectionError(m_socket->errorString());
}