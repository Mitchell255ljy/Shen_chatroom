#include "Chat.h"

#include "controller/ClientController.h"

#include <QListWidget>
#include <QTextEdit>
#include <QLineEdit>
#include <QPushButton>
#include <QMessageBox>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QListWidgetItem>
#include <QMouseEvent>
#include <QFont>


Chat::Chat(
    ClientController* controller)
    : m_chatDisplay(nullptr),
    m_messageEdit(nullptr),
    m_sendButton(nullptr),
    m_logoutButton(nullptr),
    m_userList(nullptr),
    m_controller(controller),
    m_isPrivateMode(false)
{
    setupUi();
    setupConnections();
}


void Chat::onSendClicked()
{
    const QString text = m_messageEdit->text().trimmed();
    if (text.isEmpty())
    {
        return;
    }

    if (m_isPrivateMode)
    {
        appendChatMessage(QString("我 → %1").arg(m_privateTarget.nickname()), text);
        m_controller->sendPrivateChat(m_privateTarget.username(), text);
    }
    else
    {
        m_controller->sendChatMessage(text);
    }

    m_messageEdit->clear();
}


void Chat::onLogoutClicked()
{
    m_controller->logout();
}

void Chat::onUserClicked(QListWidgetItem* item)
{
    if(!item)return;
    const int targetusername=item->data(Qt::UserRole).toInt();
    const QString targetnickname=item->text();

    if (m_isPrivateMode && m_privateTarget.username() == targetusername)
    {
        m_isPrivateMode = false;
        m_privateTarget = User();
        m_messageEdit->setPlaceholderText("请输入消息");
        appendSystemMessage("已退出私聊模式，回到公共聊天室");
        return;
    }

    m_isPrivateMode=true;
    m_privateTarget=User(targetusername,targetnickname,true);
    m_messageEdit->setPlaceholderText(
        QString("正在私聊 [%1]（再次点击可退出）").arg(targetnickname));
    appendSystemMessage(QString("已进入与 %1 的私聊模式").arg(targetnickname));
}

void Chat::onChatMessageReceived(const ChatMessage& message)//sig:ClientController::chatMessageReceived
{
    appendChatMessage(message.senderNickname(),message.content());
}


void Chat::onPrivateChatMessageReceived(const ChatMessage& message)
{
    appendChatMessage(
        QString("私聊 | %1").arg(message.senderNickname()),
        message.content());
}


void Chat::onUserJoined(const User& user)//sig:ClientController::userJoined
{
    appendSystemMessage(user.nickname()+"加入了聊天室");
    // appendSystemMessage(user.nickname()+ " 加入了聊天室");
}


void Chat::onUserLeft(const User& user)//sig:ClientController::userLeft
{
    appendSystemMessage(user.nickname()+"离开了聊天室");
    // appendSystemMessage(user.nickname()+ " 离开了聊天室");
}


void Chat::onUserListUpdated()//sig:ClientController::userListUpdated
{
    updateUserList();
}


void Chat::onNetworkError(
    const QString& error)
{
    // TODO：
    //
    // 显示网络错误。

    Q_UNUSED(error);
}


void Chat::setupUi()
{
    setObjectName("ChatWindow");
    setWindowFlag(Qt::FramelessWindowHint, true);     // 去掉系统标题栏
    setAttribute(Qt::WA_TranslucentBackground);       // 圆角外透明
    setWindowTitle("ChatRoom");
    resize(880, 560);                                 // 聊天窗较大，不用 setFixedSize

    // ==================== 背景容器（负责圆角黑底）====================
    auto *bg = new QWidget(this);
    bg->setObjectName("bg");
    bg->setGeometry(0, 0, width(), height());

    // ==================== 自定义标题栏（最小化 / 关闭）====================
    auto *titleBar = new QWidget(bg);
    titleBar->setObjectName("titleBar");
    titleBar->setFixedHeight(32);

    auto *titleBarLayout = new QHBoxLayout(titleBar);
    titleBarLayout->setContentsMargins(12, 0, 8, 0);
    titleBarLayout->setSpacing(8);

    // 左侧窗口标题文字
    auto *windowTitle = new QLabel("神的聊天室", titleBar);
    windowTitle->setObjectName("windowTitle");
    QFont wtFont = windowTitle->font();
    wtFont.setBold(true);
    windowTitle->setFont(wtFont);

    auto *minimizeBtn = new QPushButton("—", titleBar);
    minimizeBtn->setObjectName("minBtn");
    minimizeBtn->setFixedSize(24, 24);
    minimizeBtn->setCursor(Qt::PointingHandCursor);

    auto *closeBtn = new QPushButton("✕", titleBar);
    closeBtn->setObjectName("closeBtn");
    closeBtn->setFixedSize(24, 24);
    closeBtn->setCursor(Qt::PointingHandCursor);

    titleBarLayout->addWidget(windowTitle);
    titleBarLayout->addStretch();                     // 弹簧在中间，按钮靠右
    titleBarLayout->addWidget(minimizeBtn);
    titleBarLayout->addWidget(closeBtn);

    connect(minimizeBtn, &QPushButton::clicked, this, &QWidget::showMinimized);
    connect(closeBtn,    &QPushButton::clicked, this, &QWidget::close);

    // ==================== 聊天记录显示区 ====================
    m_chatDisplay = new QTextEdit(bg);
    m_chatDisplay->setReadOnly(true);
    m_chatDisplay->setObjectName("chatDisplay");

    // ==================== 消息输入框 ====================
    m_messageEdit = new QLineEdit(bg);
    m_messageEdit->setPlaceholderText("请输入消息");
    m_messageEdit->setMaxLength(2000);
    m_messageEdit->setFixedHeight(36);

    // ==================== 发送 / 退出按钮 ====================
    m_sendButton = new QPushButton("发送", bg);
    m_sendButton->setFixedHeight(36);
    m_sendButton->setFixedWidth(72);
    m_sendButton->setObjectName("loginBtn");          // 复用蓝色主按钮样式
    m_sendButton->setCursor(Qt::PointingHandCursor);

    m_logoutButton = new QPushButton("退出", bg);
    m_logoutButton->setFixedHeight(36);
    m_logoutButton->setFixedWidth(72);
    m_logoutButton->setObjectName("registerBtn");     // 复用深色次按钮样式
    m_logoutButton->setCursor(Qt::PointingHandCursor);

    // ==================== 右侧在线用户列表 ====================
    m_onlineLabel = new QLabel("在线用户（0）", bg);
    m_onlineLabel->setObjectName("onlineLabel");

    m_userList = new QListWidget(bg);
    m_userList->setObjectName("userList");
    m_userList->setFixedWidth(200);

    // ==================== 输入行 ====================
    auto *inputRow = new QHBoxLayout;
    inputRow->setSpacing(8);
    inputRow->addWidget(m_messageEdit, 1);
    inputRow->addWidget(m_sendButton);
    inputRow->addWidget(m_logoutButton);

    // ==================== 左侧：聊天区 + 输入行 ====================
    auto *leftLayout = new QVBoxLayout;
    leftLayout->setSpacing(10);
    leftLayout->addWidget(m_chatDisplay, 1);
    leftLayout->addLayout(inputRow);

    // ==================== 右侧：在线列表 ====================
    auto *rightLayout = new QVBoxLayout;
    rightLayout->setSpacing(8);
    rightLayout->addWidget(m_onlineLabel);
    rightLayout->addWidget(m_userList, 1);

    // ==================== 内容区（左右分栏）====================
    auto *contentLayout = new QHBoxLayout;
    contentLayout->setSpacing(12);
    contentLayout->addLayout(leftLayout, 1);
    contentLayout->addLayout(rightLayout);

    // ==================== 根布局（挂在 bg 上）====================
    auto *rootLayout = new QVBoxLayout(bg);
    rootLayout->setContentsMargins(12, 6, 12, 12);
    rootLayout->setSpacing(0);

    rootLayout->addWidget(titleBar);
    rootLayout->addSpacing(8);
    rootLayout->addLayout(contentLayout, 1);           // 内容区占满剩余空间

    // ==================== 样式：黑色圆角主题 ====================
    setStyleSheet(R"(
        /* 背景容器：黑色 + 圆角 */
        QWidget#bg {
            background-color: #1e1e1e;
            border-radius: 16px;
            border: 1px solid #333333;
        }

        QWidget#titleBar { background: transparent; }

        QLabel#windowTitle {
            color: #ffffff;
            font-size: 13px;
        }

        /* 最小化按钮 */
        QPushButton#minBtn {
            border: none;
            border-radius: 12px;
            background: transparent;
            color: #cccccc;
            font-size: 14px;
        }
        QPushButton#minBtn:hover { background: rgba(255,255,255,0.15); }

        /* 关闭按钮：悬停变红 */
        QPushButton#closeBtn {
            border: none;
            border-radius: 12px;
            background: transparent;
            color: #cccccc;
            font-size: 14px;
        }
        QPushButton#closeBtn:hover { background: #e81123; color: #ffffff; }

        /* 聊天记录显示区 */
        QTextEdit#chatDisplay {
            background-color: rgba(255, 255, 255, 0.05);
            border: 1px solid rgba(255, 255, 255, 0.12);
            border-radius: 10px;
            padding: 8px;
            color: #f0f0f0;
            font-size: 13px;
            selection-background-color: #4a90e2;
            selection-color: #ffffff;
        }

        /* 输入框：深色玻璃风 */
        QLineEdit {
            background-color: rgba(255, 255, 255, 0.08);
            border: 1px solid rgba(255, 255, 255, 0.18);
            border-radius: 10px;
            padding: 0 12px;
            font-size: 13px;
            color: #f0f0f0;
            selection-background-color: #4a90e2;
            selection-color: #ffffff;
        }
        QLineEdit:focus {
            background-color: rgba(255, 255, 255, 0.14);
            border: 1px solid #4a90e2;
        }

        /* 在线用户标题 */
        QLabel#onlineLabel {
            color: #aaaaaa;
            font-size: 12px;
        }

        /* 在线用户列表 */
        QListWidget#userList {
            background-color: rgba(255, 255, 255, 0.05);
            border: 1px solid rgba(255, 255, 255, 0.12);
            border-radius: 10px;
            padding: 4px;
            color: #f0f0f0;
            font-size: 13px;
            outline: none;
        }
        QListWidget#userList::item {
            padding: 6px 8px;
            border-radius: 6px;
        }
        QListWidget#userList::item:hover {
            background-color: rgba(255, 255, 255, 0.1);
        }
        QListWidget#userList::item:selected {
            background-color: #4a90e2;
            color: #ffffff;
        }

        /* 主按钮：实色蓝 */
        QPushButton#loginBtn {
            background-color: #4a90e2;
            border: none;
            border-radius: 10px;
            color: #ffffff;
            font-size: 13px;
            font-weight: bold;
        }
        QPushButton#loginBtn:hover   { background-color: #357ab8; }
        QPushButton#loginBtn:pressed { background-color: #2c6394; }

        /* 次要按钮：深色玻璃 */
        QPushButton#registerBtn {
            background-color: rgba(255, 255, 255, 0.1);
            border: 1px solid rgba(255, 255, 255, 0.2);
            border-radius: 10px;
            color: #dddddd;
            font-size: 13px;
        }
        QPushButton#registerBtn:hover   { background-color: rgba(255, 255, 255, 0.18); }
        QPushButton#registerBtn:pressed { background-color: rgba(255, 255, 255, 0.25); }

        /* 滚动条美化（聊天区 / 列表）*/
        QScrollBar:vertical {
            background: transparent;
            width: 8px;
            margin: 4px 2px 4px 2px;
        }
        QScrollBar::handle:vertical {
            background: rgba(255, 255, 255, 0.2);
            border-radius: 4px;
            min-height: 24px;
        }
        QScrollBar::handle:vertical:hover {
            background: rgba(255, 255, 255, 0.35);
        }
        QScrollBar::add-line:vertical, QScrollBar::sub-line:vertical {
            height: 0;
        }
        QScrollBar::add-page:vertical, QScrollBar::sub-page:vertical {
            background: transparent;
        }
    )");
}

void Chat::mousePressEvent(QMouseEvent *e)
{
    if (e->button() == Qt::LeftButton)
        m_dragPos = e->globalPos() - frameGeometry().topLeft();   // Qt6: e->globalPosition().toPoint()
    QWidget::mousePressEvent(e);
}

void Chat::mouseMoveEvent(QMouseEvent *e)
{
    if (e->buttons() & Qt::LeftButton)
        move(e->globalPos() - m_dragPos);                          // Qt6: e->globalPosition().toPoint()
    QWidget::mouseMoveEvent(e);
}

void Chat::setupConnections()
{
    // 发送按钮 / 回车发送
    connect(m_sendButton, &QPushButton::clicked,this, &Chat::onSendClicked);
    connect(m_messageEdit, &QLineEdit::returnPressed,this, &Chat::onSendClicked);

    // 退出按钮
    connect(m_logoutButton, &QPushButton::clicked,this, &Chat::onLogoutClicked);

    // Controller 业务信号 -> UI 更新
    connect(m_controller, &ClientController::chatMessageReceived,this, &Chat::onChatMessageReceived);
    connect(m_controller,&ClientController::privateChatMessageReceived,this,&Chat::onPrivateChatMessageReceived);
    connect(m_controller, &ClientController::userJoined,this, &Chat::onUserJoined);
    connect(m_controller, &ClientController::userLeft,this, &Chat::onUserLeft);
    connect(m_controller, &ClientController::userListUpdated,this, &Chat::onUserListUpdated);

    // 网络异常提示
    connect(m_controller, &ClientController::networkError,this, [this](const QString& reason) {QMessageBox::warning(this, "网络错误", reason);});

    connect(m_userList,&QListWidget::itemClicked,this,&Chat::onUserClicked);
}


void Chat::appendChatMessage(const QString& nickname,const QString& content)
{
    const QString line = QString("[%1] %2：%3")
                             .arg(QDateTime::currentDateTime().toString("HH:mm:ss"))
                             .arg(nickname)
                             .arg(content);
    m_chatDisplay->append(line);
}


void Chat::appendSystemMessage(
    const QString& content)
{
    m_chatDisplay->append(
        QString("<span style='color:gray;'>[%1] [系统] %2</span>")
            .arg(QDateTime::currentDateTime().toString("HH:mm:ss"))
            .arg(content));
    Q_UNUSED(content);
}


void Chat::updateUserList()
{
    const QList<User> users = m_controller->onlineUsers();
    m_userList->clear();
    for (const User& user : users)
    {
        auto* item = new QListWidgetItem(user.nickname());
        item->setData(Qt::UserRole, user.username());   // 存 username
        m_userList->addItem(item);
    }
    m_onlineLabel->setText(QString("在线用户（%1）").arg(users.size()));

}