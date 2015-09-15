#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QDebug>
#include <assert.h>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    if(QSslSocket::supportsSsl())
        qDebug() << "ok...\n";
    else
        qDebug() << "bad \n";

    m_server = new SslServer(this);
    m_server->listen(QHostAddress::Any, ui->port->text().toInt());
    connect(m_server, SIGNAL(newConnection()), this, SLOT(addConnection()));

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

    socket->setLocalCertificate("../certificate/server.crs");
    socket->setPrivateKey("../certificate/server.key");

    qDebug() << " isOpen: " << socket->isOpen() << "\n" <<
                "isReadable: " << socket->isReadable() << "\n" <<
                "isSequential: " << socket->isSequential() << "\n" <<
                "isTextModeEnamled: " << socket->isTextModeEnabled() << "\n" <<
                "isValid: " << socket->isValid() << "\n" <<
                "isWriteble: " << socket->isWritable() << "\n";

    socket->write("some text");
}

void MainWindow::write()
{
}
