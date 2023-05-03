
#ifndef CLIENT_H
#define CLIENT_H

#include <QMainWindow>
#include <QtNetwork>

QT_BEGIN_NAMESPACE
namespace Ui { class Client; }
QT_END_NAMESPACE

class Client : public QMainWindow
{
    Q_OBJECT

public:
    Client(QWidget *parent = nullptr);
    ~Client();

private:
    Ui::Client *ui;
    QTcpSocket *socket;
    QString socketIP;
    bool connected = false;

    void sendMessage();
    void connectServer();
    void disconnectServer();
    void handleMsgLog();
    void handleSysLog(QString log);
};

#endif // CLIENT_H
