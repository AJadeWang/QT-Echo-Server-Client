
#include <QDateTime>
//#include <QtNetwork>

#include "client.h"
#include "ui_client.h"

Client::Client(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::Client)
{    
    ui->setupUi(this);

    // button connections
    QPushButton::connect(ui->sendMsgButton, &QPushButton::clicked,
                         this, &Client::sendMessage);
    QPushButton::connect(ui->connectButton, &QPushButton::clicked,
                         this, &Client::connectServer);
    QPushButton::connect(ui->disconnectButton, &QPushButton::clicked,
                         this, &Client::disconnectServer);
    QPushButton::connect(ui->sendMsgButton, &QPushButton::clicked,
                         this, &Client::handleMsgLog);
}

Client::~Client()
{
    delete ui;
}


// ---- Ui Input Methods ---- //
void Client::sendMessage()
{
    if (!this->connected){
        return;
    }

    QString msg = this->ui->msgEditor->text();
    handleSysLog(msg);

    // formatted to <Client ID>|<message to server>
    QString output = QString("%1|%2").arg(this->ui->clientID->text(),msg);
    socket->write(output.toUtf8());
    socket->flush();
}

void Client::connectServer()
{
    this->socket = new QTcpSocket(this);
    this->socketIP= this->ui->serverAdress->text();
    const QHostAddress serverIP(this->socketIP);
    socket->connectToHost(serverIP, 1502);  //port hard coded to 1502
    if (!socket->waitForConnected()){
        qDebug() << "Failed to connect to server (Client)";
        return;
    }

    handleSysLog(QString("Connected to server ").append(this->socketIP));
    QString output = QString("%1||").arg(this->ui->clientID->text());
    socket->write(output.toUtf8());
    socket->flush();

    connect(socket, &QTcpSocket::readyRead, this, &Client::handleMsgLog);
    connect(socket, &QTcpSocket::disconnected, this, &Client::disconnectServer);

    this->connected = true;
}

void Client::disconnectServer()
{
    disconnect(socket, &QTcpSocket::readyRead, this, &Client::handleMsgLog);
    disconnect(socket, &QTcpSocket::disconnected, this, &Client::disconnectServer);
    handleSysLog(QString("Disconnected from server ").append(this->socketIP));
    this->socket = nullptr;
    this->socketIP = NULL;
    this->connected = false;
}

void Client::handleMsgLog()
{
    if (!this->socket) {
        qWarning() << "Unable to find socket (Client)";
        return;
    }

    QString data = QString(this->socket->readAll());
    // make sure all the data bas been passed formatted to:
    // <Characters in Message>@<Message>
    QStringList listData = data.split("@");
    if (listData.size() < 2){
        return;
    }
    int bufferSize = listData[0].toInt();
    if (listData[1].size() != bufferSize){
        return;
    }

    QDateTime currentTime = QDateTime::currentDateTime();
    QString msg = currentTime.toString("hh:mm:ss: ");
    msg.append(listData[1]);

    this->ui->msgLogBrowser->append(msg);
}

void Client::handleSysLog(QString log)
{
    QDateTime currentTime = QDateTime::currentDateTime();
    QString msg = currentTime.toString("hh:mm:ss: ");
    msg.append(log);

    this->ui->sysLogBrowser->append(msg);
}
