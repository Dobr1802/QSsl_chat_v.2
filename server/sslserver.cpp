#include "sslserver.h"
#include <QSslSocket>

SslServer::SslServer(QObject *parent) : QTcpServer(parent) {}

void SslServer::incomingConnection(int socketDescriptor)
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
