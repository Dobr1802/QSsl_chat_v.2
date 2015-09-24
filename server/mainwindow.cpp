#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QDebug>
#include <cassert>
#include <QSslCertificate>
#include <QFile>
#include <QFileDialog>

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent), ui(new Ui::MainWindow), m_server(new SslServer(this))
{
    ui->setupUi(this);

    m_certificate = ui->certLineEdit->text();
    m_key = ui->keyLineEdit->text();

    if(QSslSocket::supportsSsl())
    {
        //Connect buttons.
        connect(ui->userCertButton, &QAbstractButton::clicked, [this](){
            QString path = QFileDialog::getOpenFileName(this, "Select key file");
            if (!path.isEmpty())
            {
                QFile file(path);
                file.open(QIODevice::ReadWrite);
                m_clients_certificates.append(QSslCertificate(file.readAll()));
                file.close();
            }
        });
        connect(ui->certificateOpenButton, &QAbstractButton::clicked, [this](){
            m_certificate = QFileDialog::getOpenFileName(this, "Select certificate file");
            ui->certLineEdit->setText(m_certificate);
        });
        connect(ui->keyOpenButton, &QAbstractButton::clicked, [this](){
            m_key = QFileDialog::getOpenFileName(this, "Select keyfile");
            ui->keyLineEdit->setText(m_key);
        });
        connect(ui->starServer, &QAbstractButton::clicked, [this](){
            if (!ui->port->text().isEmpty())
            {
                m_server->listen(QHostAddress::Any, ui->port->text().toInt());
                ui->logTextEdit->append(QString("Server listening on %1 port").arg(ui->port->text()));
                connect(m_server, SIGNAL(newConnection()), this, SLOT(addConnection()));
            }
        });
    }
    else
        ui->logTextEdit->setText("SSL does not support.");
}

MainWindow::~MainWindow()
{
    delete ui;
    delete m_server;
}

void MainWindow::addConnection()
{
    QSslSocket *socket = dynamic_cast<QSslSocket *>(m_server->nextPendingConnection());
    assert(socket);

    //Connect sockets signals.
    connect(socket, &QSslSocket::encrypted, [socket, this](){
        bool isKnown = false;
        foreach (QSslCertificate cert, m_clients_certificates)
        {
            if (socket->peerCertificate() == cert)
            {
                isKnown = true;
            }
        }
        if (isKnown)
        {
            ui->logTextEdit->append(QString("New connection %1:%2 ")
                                    .arg(socket->peerAddress().toString())
                                    .arg(socket->peerPort()));
            m_sockets.append(socket);
            QByteArray block = "HTTP/1.0 200 Ok\r\n"
           "Content-Type: text/html; charset=\"utf-8\"\r\n"
           "\r\n"
           "FUUUUU!"
           "\r\n";
            socket->write(block);
        }
        else
        {
            ui->logTextEdit->append(QString("Peer ignored %1:%2 ")
                                    .arg(socket->peerAddress().toString())
                                    .arg(socket->peerPort()));
            socket->close();
        }
    });
    connect(socket, &QSslSocket::disconnected, [socket, this](){
        ui->logTextEdit->append(QString("Peer %1:%2 disconnected.")
                                .arg(socket->peerAddress().toString())
                                .arg(socket->peerPort()));
    });
    connect(socket, &QSslSocket::readyRead, [socket, this](){
        ui->logTextEdit->append(QString("Msg from %1:%2 : %3")
                                .arg(socket->peerAddress().toString())
                                .arg(socket->peerPort())
                                .arg(socket->readAll().constData()));
    });
    connect(socket, SIGNAL(sslErrors(const QList<QSslError> &)), this, SLOT(sslErr(const QList<QSslError> &)));
    connect(socket, SIGNAL(error(QAbstractSocket::SocketError)), this, SLOT(somthWrong(QAbstractSocket::SocketError)));

    socket->ignoreSslErrors();

    socket->setLocalCertificate(m_certificate);
    socket->setPrivateKey(m_key);
    socket->setPeerVerifyMode(QSslSocket::VerifyPeer);
    socket->startServerEncryption();
}

void MainWindow::sslErr(const QList<QSslError> &errors)
{
    QSslSocket *socket = dynamic_cast<QSslSocket *>(sender());
    assert(socket);

    ui->logTextEdit->append(QString("Errors:"));
    foreach (auto err, errors) {
        ui->logTextEdit->append(err.errorString());
    }
    socket->ignoreSslErrors();
}

void MainWindow::somthWrong(QAbstractSocket::SocketError err)
{
    qDebug() << err;
}
