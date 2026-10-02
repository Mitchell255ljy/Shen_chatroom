#pragma once

#include <QObject>

class ClientController;
class Login;
class Register;
class Chat;


class WindowManager : public QObject
{
    Q_OBJECT

public:

    explicit WindowManager(
        ClientController* controller);

    ~WindowManager();

    void showLoginWindow();


private slots:

    void showRegisterWindow();
    void backToLoginWindow();
    void onLoginSucceeded();
    void onLoggedOut();

private:

    void setupConnections();
    void hideAllWindows();


private:

    ClientController* m_controller;
    Login* m_loginWindow;
    Register* m_registerWindow;
    Chat* m_chatWindow;
};