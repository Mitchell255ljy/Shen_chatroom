#pragma once

#include <QWidget>

class QLineEdit;
class QPushButton;
class ClientController;


class Login : public QWidget
{
    Q_OBJECT

public:

    explicit Login(ClientController* controller);


signals:

    void registerRequested();//slot:WindowManager::showRegisterWindow


private slots:

    void onLoginClicked();

    void onLoginSucceeded();//sig:ClientController::loginSucceeded

    void onLoginFailed(const QString& reason);//sig:ClientController::loginFailed



private:

    void setupUi();

    void setupConnections();

    void clearPassword();

protected:
    void mousePressEvent(QMouseEvent* e) override;
    void mouseMoveEvent(QMouseEvent* e) override;



private:

    QPoint m_dragPos;

    QLineEdit* m_usernameEdit;

    QLineEdit* m_passwordEdit;

    QPushButton* m_loginButton;

    QPushButton* m_registerButton;

    ClientController* m_controller;
};