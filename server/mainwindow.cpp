#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QDebug>
#include <cassert>
#include <QSslCertificate>
#include <QFile>

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent), ui(new Ui::MainWindow), m_server(new SslServer(this))
{
    ui->setupUi(this);

    if(QSslSocket::supportsSsl())
        qDebug() << "ok...\n";
    else
        qDebug() << "bad \n";

    m_server->listen(QHostAddress::Any, ui->port->text().toInt());
    connect(m_server, SIGNAL(newConnection()), this, SLOT(addConnection()));

    QFile file("/home/kindnes/projects/client_server/certificates/client.crt");
    file.open(QIODevice::ReadWrite);
    m_clients_certificates.append(QSslCertificate(file.readAll()));
    file.close();

    ui->startStopServer->setEnabled(false);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::addConnection()
{
    QSslSocket *socket = dynamic_cast<QSslSocket *>(m_server->nextPendingConnection());
    assert(socket);
    socket->setPeerVerifyMode(QSslSocket::VerifyPeer);

    connect(socket, &QSslSocket::encrypted, [socket, this](){
        socket->write(" ||||||| ");
    });

    connect(socket, SIGNAL(sslErrors(const QList<QSslError> &)), this, SLOT(sslErr(const QList<QSslError> &)));
    connect(socket, SIGNAL(error(QAbstractSocket::SocketError)), this, SLOT(somthWrong(QAbstractSocket::SocketError)));

    QList<QSslError> errors;
    errors.append(QSslError::CertificateUntrusted);
    errors.append(QSslError::SelfSignedCertificate);
    socket->ignoreSslErrors(errors);

    socket->setLocalCertificate(ui->sertLineEdit->text());
    socket->setPrivateKey(ui->keyLineEdit->text());
    socket->startServerEncryption();
}

void MainWindow::sslErr(const QList<QSslError> &err)
{
    qDebug() << err;
}

void MainWindow::somthWrong(QAbstractSocket::SocketError err)
{
    qDebug() << "some bad..." << err;
}
