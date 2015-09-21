#ifndef SSLSERVER_H
#define SSLSERVER_H

#include <QTcpServer>

class SslServer : public QTcpServer
{
    Q_OBJECT
public:
    explicit SslServer(QObject *parent = 0);

void incomingConnection(qintptr socketDescriptor);

};

#endif // SSLSERVER_H
