#include "WindowManager.h"

#include "Login.h"
#include "Register.h"
#include "Chat.h"

#include "controller/ClientController.h"
#include<QMessageBox>

WindowManager::WindowManager(
    ClientController* controller)
    : m_controller(controller),
    m_loginWindow(nullptr),
    m_registerWindow(nullptr),
    m_chatWindow(nullptr)
{
    m_loginWindow =
        new Login(m_controller);

    m_registerWindow =
        new Register(m_controller);

    m_chatWindow =
        new Chat(m_controller);

    setupConnections();
}


WindowManager::~WindowManager()
{
    delete m_loginWindow;
    delete m_registerWindow;
    delete m_chatWindow;
}


void WindowManager::showLoginWindow()
{
    hideAllWindows();

    if (m_loginWindow)
    {
        m_loginWindow->show();
        m_loginWindow->raise();
        m_loginWindow->activateWindow();
    }
}


void WindowManager::showRegisterWindow()
{

    if (m_loginWindow)
    {
        m_loginWindow->hide();
    }

    if (m_registerWindow)
    {
        m_registerWindow->show();
        m_registerWindow->raise();
        m_registerWindow->activateWindow();
    }
}


void WindowManager::backToLoginWindow()
{

    if (m_registerWindow)
    {
        m_registerWindow->hide();
    }

    showLoginWindow();
}


void WindowManager::onLoginSucceeded()
{

    if (m_loginWindow)
    {
        m_loginWindow->hide();
    }

    if (m_registerWindow)
    {
        m_registerWindow->hide();
    }

    if (m_chatWindow)
    {
        m_chatWindow->show();
        m_chatWindow->raise();
        m_chatWindow->activateWindow();
    }
     m_controller->requestUserList();
}


void WindowManager::onLoggedOut()
{

    if (m_chatWindow)
    {
        m_chatWindow->hide();
    }

    showLoginWindow();
}


void WindowManager::hideAllWindows()
{

    if (m_loginWindow)
    {
        m_loginWindow->hide();
    }

    if (m_registerWindow)
    {
        m_registerWindow->hide();
    }

    if (m_chatWindow)
    {
        m_chatWindow->hide();
    }
}


void WindowManager::setupConnections()
{

    connect(m_loginWindow, &Login::registerRequested,
            this, &WindowManager::showRegisterWindow);
    connect(m_registerWindow, &Register::backToLoginRequested,
            this, &WindowManager::backToLoginWindow);


    connect(m_controller, &ClientController::loginSucceeded,
            this, &WindowManager::onLoginSucceeded);
    connect(m_controller, &ClientController::loggedOut,
            this, &WindowManager::onLoggedOut);


    connect(m_controller, &ClientController::networkError,
            this, [this](const QString& error) {
                QWidget* parent = m_chatWindow->isVisible()
                ? static_cast<QWidget*>(m_chatWindow)
                : m_loginWindow->isVisible()
                    ? static_cast<QWidget*>(m_loginWindow)
                    : static_cast<QWidget*>(m_registerWindow);
                QMessageBox::warning(parent, "网络错误", error);
            });


    connect(m_controller, &ClientController::disconnected,
            this, [this]() {
                if (m_chatWindow->isVisible()) {
                    onLoggedOut();
                }
            });
}