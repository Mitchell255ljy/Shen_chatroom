#include <QApplication>

#include "controller/ClientController.h"
#include "ui/WindowManager.h"
#include<QDir>
#include<QFile>

int main(
    int argc,
    char* argv[])
{
    QApplication app(
        argc,
        argv);
    ClientController controller;

    WindowManager windowManager(&controller);

    qDebug() << "当前工作目录:" << QDir::currentPath();
    QFile file("../../server.conf");
    qDebug() << "文件是否存在:" << file.exists();

    std::unordered_map<std::string,std::string> configmap=ConfigManager::readServerConfig();
    QString host=QString::fromStdString(configmap["HOST"]);
    quint16 port=QString::fromStdString(configmap["PORT"]).toInt();
    controller.connectToServer(host,port);

    windowManager.showLoginWindow();

    return app.exec();

}