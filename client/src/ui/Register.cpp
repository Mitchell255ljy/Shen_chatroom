#include "Register.h"

#include "controller/ClientController.h"

#include <QLineEdit>
#include <QPushButton>
#include <QMessageBox>
#include <QVBoxLayout>
#include <QLabel>
#include<QFormLayout>
#include<QRegularExpressionValidator>
#include<QRegularExpression>
#include<QMouseEvent>
#include<QFont>


Register::Register(
    ClientController* controller)
    : m_usernameEdit(nullptr),
    m_passwordEdit(nullptr),
    m_nicknameEdit(nullptr),
    m_registerButton(nullptr),
    m_backButton(nullptr),
    m_controller(controller)
{
    setupUi();
    setupConnections();
}


void Register::onRegisterClicked()
{
    const QString username=m_usernameEdit->text();
    const QString nickname=m_nicknameEdit->text();
    const QString password=m_passwordEdit->text();

    if (username.isEmpty() || password.isEmpty()||nickname.isEmpty())
    {
        QMessageBox::information(this, "注册提示", "输入的内容不能为空");
        return;
    }

    if (username.length() != 6) {
        QMessageBox::warning(this, "提示", "用户名必须是 6 位数字");
        m_usernameEdit->setFocus();
        return;
    }

    bool ok = false;
    username.toInt(&ok);
    if (!ok)
    {
        QMessageBox::warning(this, "提示", "用户名只能包含数字");
        m_usernameEdit->setFocus();
        return;
    }
    m_registerButton->setEnabled(true);
    m_controller->registerUser(username.toInt(),nickname,password);
}

void Register::onRegisterSucceeded()
{
    m_registerButton->setEnabled(true);
    QMessageBox::information(this, "注册提示", "注册成功，请登录");
    emit backToLoginRequested();
}

void Register::onRegisterFailed( const QString& reason)
{
    m_registerButton->setEnabled(true);
    QMessageBox::warning(this, "注册失败",reason.isEmpty() ? "注册失败，" : reason);
}

void Register::setupUi()
{
    setObjectName("RegisterWindow");
    setWindowFlag(Qt::FramelessWindowHint, true);     // 去掉系统标题栏
    setAttribute(Qt::WA_TranslucentBackground);       // 圆角外透明
    setWindowTitle("ChatRoom - 注册");
    setFixedSize(300, 420);

    // ==================== 背景容器（负责圆角黑底）====================
    auto *bg = new QWidget(this);
    bg->setObjectName("bg");
    bg->setGeometry(0, 0, width(), height());

    // ==================== 自定义标题栏（最小化 / 关闭）====================
    auto *titleBar = new QWidget(bg);
    titleBar->setObjectName("titleBar");
    titleBar->setFixedHeight(32);

    auto *titleBarLayout = new QHBoxLayout(titleBar);
    titleBarLayout->setContentsMargins(0, 0, 8, 0);
    titleBarLayout->setSpacing(8);

    auto *minimizeBtn = new QPushButton("—", titleBar);
    minimizeBtn->setObjectName("minBtn");
    minimizeBtn->setFixedSize(24, 24);
    minimizeBtn->setCursor(Qt::PointingHandCursor);

    auto *closeBtn = new QPushButton("✕", titleBar);
    closeBtn->setObjectName("closeBtn");
    closeBtn->setFixedSize(24, 24);
    closeBtn->setCursor(Qt::PointingHandCursor);

    titleBarLayout->addStretch();
    titleBarLayout->addWidget(minimizeBtn);
    titleBarLayout->addWidget(closeBtn);

    connect(minimizeBtn, &QPushButton::clicked, this, &QWidget::showMinimized);
    connect(closeBtn,    &QPushButton::clicked, this, &QWidget::close);

    // ==================== 标题 ====================
    auto *avatarLabel = new QLabel("神的聊天室", bg);
    avatarLabel->setAlignment(Qt::AlignCenter);
    QFont avatarFont = avatarLabel->font();
    avatarFont.setPointSize(24);
    avatarFont.setBold(true);
    avatarLabel->setFont(avatarFont);
    avatarLabel->setObjectName("titleLabel");

    // ==================== 输入框 ====================
    m_usernameEdit = new QLineEdit(bg);
    m_usernameEdit->setPlaceholderText("请输入用户名（6 位数字）");
    QRegularExpression rx("^\\d{6}$");
    m_usernameEdit->setValidator(new QRegularExpressionValidator(rx, this));
    m_usernameEdit->setFixedHeight(36);

    m_nicknameEdit = new QLineEdit(bg);
    m_nicknameEdit->setPlaceholderText("请输入昵称");
    m_nicknameEdit->setMaxLength(20);
    m_nicknameEdit->setFixedHeight(36);

    m_passwordEdit = new QLineEdit(bg);
    m_passwordEdit->setPlaceholderText("请输入密码");
    m_passwordEdit->setEchoMode(QLineEdit::Password);
    m_passwordEdit->setMaxLength(50);
    m_passwordEdit->setFixedHeight(36);

    // ==================== 按钮 ====================
    m_registerButton = new QPushButton("注册", bg);
    m_registerButton->setDefault(true);
    m_registerButton->setFixedHeight(36);
    m_registerButton->setFixedWidth(120);
    m_registerButton->setObjectName("loginBtn");       // 复用主按钮样式
    m_registerButton->setCursor(Qt::PointingHandCursor);

    m_backButton = new QPushButton("去登录", bg);
    m_backButton->setFixedHeight(36);
    m_backButton->setFixedWidth(120);
    m_backButton->setObjectName("registerBtn");        // 复用次按钮样式
    m_backButton->setCursor(Qt::PointingHandCursor);

    // ==================== 输入框容器（限制宽度并居中）====================
    auto *edits = new QWidget(bg);
    edits->setFixedWidth(200);
    auto *editsLayout = new QVBoxLayout(edits);
    editsLayout->setContentsMargins(0, 0, 0, 0);
    editsLayout->setSpacing(16);                       // 3 个输入框，间距略小
    editsLayout->addWidget(m_usernameEdit);
    editsLayout->addWidget(m_nicknameEdit);
    editsLayout->addWidget(m_passwordEdit);

    auto *editsRow = new QHBoxLayout();
    editsRow->addStretch();
    editsRow->addWidget(edits);
    editsRow->addStretch();

    // ==================== 按钮行（垂直居中排列）====================
    auto *btnRow = new QVBoxLayout();
    btnRow->setSpacing(16);
    btnRow->addWidget(m_registerButton, 0, Qt::AlignHCenter);
    btnRow->addWidget(m_backButton,     0, Qt::AlignHCenter);

    // ==================== 根布局（挂在 bg 上）====================
    auto *rootLayout = new QVBoxLayout(bg);
    rootLayout->setContentsMargins(12, 6, 12, 30);
    rootLayout->setSpacing(0);

    rootLayout->addWidget(titleBar);
    rootLayout->addSpacing(10);
    rootLayout->addWidget(avatarLabel);
    rootLayout->addSpacing(30);                        // 3 个输入框，留白略减
    rootLayout->addLayout(editsRow);
    rootLayout->addStretch();
    rootLayout->addLayout(btnRow);

    // ==================== 样式：黑色圆角主题 ====================
    setStyleSheet(R"(
        /* 背景容器：黑色 + 圆角 */
        QWidget#bg {
            background-color: #1e1e1e;
            border-radius: 16px;
            border: 1px solid #333333;
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

        /* 关闭按钮：悬停变红 */
        QPushButton#closeBtn {
            border: none;
            border-radius: 12px;
            background: transparent;
            color: #cccccc;
            font-size: 14px;
        }
        QPushButton#closeBtn:hover { background: #e81123; color: #ffffff; }

        /* 标题 */
        QLabel#titleLabel {
            color: #ffffff;
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
    )");
}

void Register::mousePressEvent(QMouseEvent *e)
{
    if (e->button() == Qt::LeftButton)
        m_dragPos = e->globalPos() - frameGeometry().topLeft();   // Qt6: e->globalPosition().toPoint()
    QWidget::mousePressEvent(e);
}

void Register::mouseMoveEvent(QMouseEvent *e)
{
    if (e->buttons() & Qt::LeftButton)
        move(e->globalPos() - m_dragPos);                          // Qt6: e->globalPosition().toPoint()
    QWidget::mouseMoveEvent(e);
}

void Register::setupConnections()
{
    connect(m_registerButton, &QPushButton::clicked,this, &Register::onRegisterClicked);
    connect(m_backButton, &QPushButton::clicked,this, &Register::backToLoginRequested);
    // 业务结果反馈
    connect(m_controller, &ClientController::registerSucceeded,this, &Register::onRegisterSucceeded);
    connect(m_controller, &ClientController::registerFailed,this, &Register::onRegisterFailed);
    // 网络异常提示
    connect(m_controller, &ClientController::networkError,this, [this](const QString& reason) {m_registerButton->setEnabled(true);QMessageBox::warning(this, "网络错误", reason);});
}