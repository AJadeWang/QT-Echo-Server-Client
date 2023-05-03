#include <QtNetwork>

#include "server.h"

Server::Server(QObject *parent)
    :QObject(parent)
{
    QMutexLocker locker(&mutex);
}

// ---- Class methods ---- //
void Server::startServer(QString ip, quint16 port)
{
    //reading data
    mutex.lock();
    this->ip = ip;
    this->port = port;
    const QHostAddress serverIP(ip);
    const quint16 serverPort = port;
    this->server = new QTcpServer(this);
    mutex.unlock();

    // starting new server
    connect(this->server, &QTcpServer::newConnection, this, &Server::handleClient);

    if (!this->server->listen(serverIP, serverPort)) {
        qWarning() << "Failed to start server:" << this->server->errorString();
    }
}


// private methods //
void Server::sendData(QTcpSocket *socket, QString data)
{
    int sizeOfData = data.size();
    if (sizeOfData > 100){
        sizeOfData = 100;
        data = data.left(100);
    }
    QString output = QString("%1@%2").arg(QString::number(sizeOfData), data);

    socket->write(output.toUtf8());
    socket->flush();
}

void Server::handleClient()
{
    QTcpSocket *socket = this->server->nextPendingConnection();

    connect(socket, &QTcpSocket::readyRead, this, &Server::readData);
    connect(socket, &QTcpSocket::disconnected, this, &Server::disconnectClient);
}

void Server::readData()
{
    QTcpSocket *socket = qobject_cast<QTcpSocket*>(sender());
    if (!socket) {
        qWarning() << "Unable to find socket on read";
        return;
    }

    QString data = QString(socket->readAll());
    QStringList listData = data.split("|");
    // index 0 is the user id
    // index 1 is the message

    QString output;
    if (listData.size() > 2){
        output = QString("Client: %1 connected to server %2").arg(listData[0], this->ip);
    }else{
        output = QString("%1 [ACK:%2]").arg(listData[1],listData[0]);
    }

    sendData(socket, output);
}

void Server::disconnectClient()
{
    QTcpSocket *socket = qobject_cast<QTcpSocket*>(sender());
    disconnect(socket, &QTcpSocket::readyRead, this, &Server::readData);
    disconnect(socket, &QTcpSocket::disconnected, this, &Server::disconnectClient);
}
