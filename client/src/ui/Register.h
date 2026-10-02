#pragma once

#include <QWidget>

class QLineEdit;
class QPushButton;
class ClientController;


class Register : public QWidget
{
    Q_OBJECT

public:

    explicit Register(ClientController* controller);


signals:

    void backToLoginRequested();//slot:WindowManager::backToLoginWindow


private slots:

    void onRegisterClicked();

    void onRegisterSucceeded();//signal:ClientController::registerSucceeded

    void onRegisterFailed(
        const QString& reason);//signal:ClientController::registerFailed


private:

    void setupUi();

    void setupConnections();

protected:
    void mousePressEvent(QMouseEvent *e) override;
    void mouseMoveEvent(QMouseEvent *e) override;



private:

    QPoint m_dragPos;

    QLineEdit* m_usernameEdit;

    QLineEdit* m_passwordEdit;

    QLineEdit* m_nicknameEdit;

    QPushButton* m_registerButton;

    QPushButton* m_backButton;

    ClientController* m_controller;
};