
#ifndef SERVER_H
#define SERVER_H

#include <QString>
#include <QThread>
#include <QMutex>
#include <QObject>
#include <QtNetwork>

class Server : public QObject
{
    Q_OBJECT;

public:
    Server(QObject *parent = nullptr);

    void startServer(QString ip, quint16 port);

private:
    QMutex mutex;
    QTcpServer *server;
    QString ip;
    quint16 port;

    void handleClient();
    void readData();
    void disconnectClient();
    void sendData(QTcpSocket *socket, QString output);
};

#endif // SERVER_H
