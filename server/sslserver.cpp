#include "sslserver.h"
#include <QSslSocket>

SslServer::SslServer(QObject *parent) : QTcpServer(parent) {}

void SslServer::incomingConnection(qintptr socketDescriptor)
{
    QSslSocket *serverSocket = new QSslSocket();
    if (serverSocket->setSocketDescriptor(socketDescriptor))
    {
        addPendingConnection(serverSocket);
    }
    else
    {
        delete serverSocket;
    }
}
