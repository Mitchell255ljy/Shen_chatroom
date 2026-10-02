#pragma once

#include <QWidget>

#include "model/User.h"
#include "model/ChatMessage.h"
#include<QLabel>
#include<QListWidget>
#include<QTextEdit>
#include<QLineEdit>
#include<QPushButton>

class ClientController;


class Chat : public QWidget
{
    Q_OBJECT

public:

    explicit Chat(ClientController* controller);


private slots:

    void onSendClicked();
    void onLogoutClicked();
    void onUserClicked(QListWidgetItem* item);
    void onChatMessageReceived(const ChatMessage& message);
    void onPrivateChatMessageReceived(const ChatMessage& message);
    void onUserJoined(const User& user);
    void onUserLeft(const User& user);
    void onUserListUpdated();
    void onNetworkError(const QString& error);


private:

    void setupUi();
    void setupConnections();
    void appendChatMessage(const QString& nickname,const QString& content);
    void appendSystemMessage(const QString& content);
    void updateUserList();

protected:
    void mousePressEvent(QMouseEvent *e) override;
    void mouseMoveEvent(QMouseEvent *e) override;


private:
    QPoint m_dragPos;
    QTextEdit* m_chatDisplay;
    QLineEdit* m_messageEdit;
    QPushButton* m_sendButton;
    QPushButton* m_logoutButton;
    QListWidget* m_userList;
    QLabel* m_onlineLabel;
    ClientController* m_controller;
    User m_privateTarget;
    bool m_isPrivateMode;
};