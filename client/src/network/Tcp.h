#pragma once

#include <QObject>
#include <QTcpSocket>
#include <QAbstractSocket>
#include <QByteArray>
#include <QString>

class Tcp : public QObject
{
    Q_OBJECT

public:

    Tcp();
    ~Tcp();

    void connectToServer(const QString& host,quint16 port);
    void disconnectFromServer();
    bool isConnected() const;
    void sendData(const QByteArray& data);

signals:

    void connected();//slot:ClientController::onConnected
    void disconnected();//slot:ClientController::onDisconnected
    void dataReceived(const QByteArray& data);//slot:ClientController::onDataReceived
    void connectionError(const QString& error);//slot:ClientController::onConnectionError

    //信号见构造函数
private slots:

    void onConnected();
    void onDisconnected();
    void onReadyRead();
    void onErrorOccurred(QAbstractSocket::SocketError error);

private:

    QTcpSocket* m_socket;
};