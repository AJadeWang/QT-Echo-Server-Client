
#include "client.h"
#include "server.h"

#include <QApplication>


int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    Server server;
    server.startServer("127.0.0.1",1502);
    Client w;
    w.show();
    return a.exec();
}
