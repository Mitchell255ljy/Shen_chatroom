#include "Login.h"

#include "controller/ClientController.h"

#include <QFont>
#include <QFormLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QMessageBox>
#include <QPushButton>
#include <QVBoxLayout>
#include<QRegularExpression>
#include<QRegularExpressionValidator>
#include<QMouseEvent>


Login::Login(ClientController* controller)
 : m_usernameEdit(nullptr),m_passwordEdit(nullptr),m_loginButton(nullptr),m_registerButton(nullptr),m_controller(controller)
{
    setupUi();
    setupConnections();
}

void Login::onLoginClicked()
{
    const QString username = m_usernameEdit->text().trimmed();
    const QString password = m_passwordEdit->text();

    if (username.isEmpty() || password.isEmpty())
    {
        QMessageBox::information(this, "登录提示", "用户名和密码不能为空");
        return;
    }

    if (username.length() != 6) {
        QMessageBox::warning(this, "提示", "用户名必须是 6 位数字");
        m_usernameEdit->setFocus();
        return;
    }

    if(password.length()>18)
    {
        QMessageBox::warning(this,"提示","密码最大为18位");
        return;
    }

    bool ok = false;
    username.toInt(&ok);
    if (!ok) {
        QMessageBox::warning(this, "提示", "用户名只能包含数字");
        m_usernameEdit->setFocus();
        return;
    }

    // 防止重复提交
    m_loginButton->setEnabled(true);
    m_controller->login(username.toInt(), password);
}


void Login::onLoginSucceeded()
{
    clearPassword();
}


void Login::onLoginFailed(const QString& reason)
{
    m_loginButton->setEnabled(true);
    QMessageBox::warning(this, "登录失败",reason.isEmpty() ? "用户名或密码错误" : reason);
    m_passwordEdit->clear();
    m_passwordEdit->setFocus();
}

void Login::setupUi()
{
    setObjectName("LoginWindow");           // 供 QSS 选择器使用
    setWindowFlag(Qt::FramelessWindowHint, true);   // ★ 去掉系统标题栏
    setAttribute(Qt::WA_TranslucentBackground);
    setWindowTitle("ChatRoom - 登录");       // 任务栏仍显示
    setFixedSize(300, 420);

    // ============ 自定义标题栏（最小化 / 关闭）============
    auto* titleBar = new QWidget(this);
    titleBar->setObjectName("titleBar");
    titleBar->setFixedHeight(32);
    auto* titleBarLayout = new QHBoxLayout(titleBar);
    titleBarLayout->setContentsMargins(0, 0, 0, 0);
    titleBarLayout->setSpacing(6);

    auto* minimizeBtn = new QPushButton("—", titleBar);   // 最小化
    minimizeBtn->setObjectName("minBtn");
    minimizeBtn->setFixedSize(24, 24);
    minimizeBtn->setCursor(Qt::PointingHandCursor);

    auto* closeBtn = new QPushButton("✕", titleBar);      // 关闭
    closeBtn->setObjectName("closeBtn");
    closeBtn->setFixedSize(24, 24);
    closeBtn->setCursor(Qt::PointingHandCursor);

    titleBarLayout->addStretch();               // 把按钮推到右边
    titleBarLayout->addWidget(minimizeBtn,0,Qt::AlignTop);
    titleBarLayout->addWidget(closeBtn,0,Qt::AlignTop);

    connect(minimizeBtn, &QPushButton::clicked, this, &QWidget::showMinimized);
    connect(closeBtn,    &QPushButton::clicked, this, &QWidget::close);

    // ============ 标题 ============
    auto* avatarLabel = new QLabel("神的聊天室", this);
    avatarLabel->setAlignment(Qt::AlignCenter);
    QFont avatarFont = avatarLabel->font();
    avatarFont.setPointSize(24);
    avatarFont.setBold(true);
    avatarLabel->setFont(avatarFont);
    avatarLabel->setObjectName("titleLabel");

    // ============ 输入框 ============
    m_usernameEdit = new QLineEdit(this);
    m_usernameEdit->setPlaceholderText("请输入用户名");
    QRegularExpression rx("^\\d{6}$");
    m_usernameEdit->setValidator(new QRegularExpressionValidator(rx, this));
    m_usernameEdit->setFixedHeight(36);

    m_passwordEdit = new QLineEdit(this);
    m_passwordEdit->setPlaceholderText("请输入密码");
    m_passwordEdit->setEchoMode(QLineEdit::Password);
    m_passwordEdit->setMaxLength(50);
    m_passwordEdit->setFixedHeight(36);

    // ============ 按钮 ============
    m_loginButton = new QPushButton("登录", this);
    m_loginButton->setDefault(true);
    m_loginButton->setFixedHeight(36);
    m_loginButton->setFixedWidth(120);
    m_loginButton->setObjectName("loginBtn");

    m_registerButton = new QPushButton("注册账号", this);
    m_registerButton->setFixedHeight(36);
    m_registerButton->setFixedWidth(120);
    m_registerButton->setObjectName("registerBtn");

    // ============ 输入框容器 ============
    auto* edits = new QWidget(this);
    edits->setFixedWidth(200);
    auto* editsLayout = new QVBoxLayout(edits);
    editsLayout->setContentsMargins(0, 0, 0, 0);
    editsLayout->setSpacing(24);
    editsLayout->addWidget(m_usernameEdit);
    editsLayout->addWidget(m_passwordEdit);

    auto* editsRow = new QHBoxLayout();
    editsRow->addStretch();
    editsRow->addWidget(edits);
    editsRow->addStretch();

    // ============ 按钮行 ============
    auto* btnRow = new QVBoxLayout();
    btnRow->setSpacing(16);
    btnRow->addWidget(m_loginButton, 0, Qt::AlignHCenter);
    btnRow->addWidget(m_registerButton, 0, Qt::AlignHCenter);

    // ============ 根布局 ============
    auto* bg = new QWidget(this);
    bg->setObjectName("bg");
    bg->setGeometry(0, 0, width(), height());   // 铺满整个窗口

    auto* rootLayout = new QVBoxLayout(bg);     // ★ 根布局挂到 bg 上
    rootLayout->setContentsMargins(12, 6, 12, 30);
    rootLayout->setSpacing(0);

    rootLayout->addWidget(titleBar);
    rootLayout->addSpacing(10);
    rootLayout->addWidget(avatarLabel);
    rootLayout->addSpacing(40);
    rootLayout->addLayout(editsRow);
    rootLayout->addStretch();
    rootLayout->addLayout(btnRow);

    // 让 bg 随窗口大小自适应（其实 setFixedSize 固定了，但更稳妥）
    bg->setFixedSize(width(), height());
    // ============ 样式 ============
    setStyleSheet(R"(
    /* ★ 背景容器：黑色 + 圆角 */
    QWidget#bg {
        background-color: #1e1e1e;
        border-radius: 16px;
        border: 1px solid #333333;   /* 可选：细边框增加层次 */
    }

    QWidget#titleBar { background: transparent; }

    /* 最小化按钮 */
    QPushButton#minBtn {
        border: none;
        border-radius: 12px;
        background: transparent;
        color: #cccccc;
        font-size: 14px;
    }
    QPushButton#minBtn:hover { background: rgba(255,255,255,0.15); }

    /* 关闭按钮 */
    QPushButton#closeBtn {
        border: none;
        border-radius: 12px;
        background: transparent;
        color: #cccccc;
        font-size: 14px;
    }
    QPushButton#closeBtn:hover { background: #e81123; color: #ffffff; }

    /* 标题：白字 + 阴影 */
    QLabel#titleLabel {
        color: #ffffff;
        text-shadow: 0 1px 3px rgba(0, 0, 0, 0.6);
    }

    /* 输入框：深色玻璃风 */
    QLineEdit {
        background-color: rgba(255, 255, 255, 0.08);   /* 深底上半透明白 */
        border: 1px solid rgba(255, 255, 255, 0.18);
        border-radius: 10px;
        padding: 0 12px;
        font-size: 13px;
        color: #f0f0f0;                                /* 浅色文字 */
        selection-background-color: #4a90e2;
        selection-color: #ffffff;
    }
    QLineEdit:focus {
        background-color: rgba(255, 255, 255, 0.14);
        border: 1px solid #4a90e2;                     /* 聚焦蓝色边框 */
    }

    /* 主按钮：实色蓝，醒目 */
    QPushButton#loginBtn {
        background-color: #4a90e2;
        border: none;
        border-radius: 10px;
        color: #ffffff;
        font-size: 13px;
        font-weight: bold;
    }
    QPushButton#loginBtn:hover  { background-color: #357ab8; }
    QPushButton#loginBtn:pressed { background-color: #2c6394; }

    /* 次要按钮：深色玻璃 */
    QPushButton#registerBtn {
        background-color: rgba(255, 255, 255, 0.1);
        border: 1px solid rgba(255, 255, 255, 0.2);
        border-radius: 10px;
        color: #dddddd;
        font-size: 13px;
    }
    QPushButton#registerBtn:hover  { background-color: rgba(255, 255, 255, 0.18); }
    QPushButton#registerBtn:pressed { background-color: rgba(255, 255, 255, 0.25); }
)");
}


void Login::setupConnections()
{
    connect(m_loginButton, &QPushButton::clicked,this, &Login::onLoginClicked);
    connect(m_registerButton, &QPushButton::clicked,this, &Login::registerRequested);
    connect(m_passwordEdit, &QLineEdit::returnPressed,this, &Login::onLoginClicked);
    connect(m_controller, &ClientController::loginSucceeded,this, &Login::onLoginSucceeded);
    connect(m_controller, &ClientController::loginFailed,this, &Login::onLoginFailed);
    connect(m_controller, &ClientController::networkError,this, [this](const QString& reason) {m_loginButton->setEnabled(true);QMessageBox::warning(this, "网络错误", reason);});
}


void Login::clearPassword()
{
    m_passwordEdit->clear();
}

void Login::mousePressEvent(QMouseEvent* e) {
    if (e->button() == Qt::LeftButton)
        m_dragPos = e->globalPos() - frameGeometry().topLeft();  // Qt6: globalPosition().toPoint()
    QWidget::mousePressEvent(e);
}

void Login::mouseMoveEvent(QMouseEvent* e) {
    if (e->buttons() & Qt::LeftButton)
        move(e->globalPos() - m_dragPos);                        // Qt6: globalPosition().toPoint()
    QWidget::mouseMoveEvent(e);
}