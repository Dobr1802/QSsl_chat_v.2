#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QFile>
#include <QDebug>
#include <cassert>
#include <QFileDialog>
#include <QSslCertificate>

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent), ui(new Ui::MainWindow), m_server(new SslServer(this)), m_clients_certificates()
{
    ui->setupUi(this);

    if(!QSslSocket::supportsSsl())
    {
        ui->logTextEdit->setText("SSL does not support.");
        return;
    }

    //Connect buttons.
    connect(ui->userCertButton, &QAbstractButton::clicked, [this](){
        QString path = QFileDialog::getOpenFileName(this, "Select key file");
        if (!path.isEmpty())
        {
            QFile file(path);
            file.open(QIODevice::ReadOnly);
            m_clients_certificates.add(file.readAll());
            file.close();
        }
    });
    connect(ui->certificateOpenButton, &QAbstractButton::clicked, [this](){
        QString certificate = QFileDialog::getOpenFileName(this, "Select certificate file");
        if (!certificate.isEmpty())
        {
            ui->certLineEdit->setText(certificate);
            m_certificate = certificate;
        }
    });
    connect(ui->keyOpenButton, &QAbstractButton::clicked, [this](){
        QString key = QFileDialog::getOpenFileName(this, "Select key file");
        if (!key.isEmpty())
        {
            ui->keyLineEdit->setText(key);
            m_key = key;
        }
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

MainWindow::~MainWindow()
{
    delete ui;
    delete m_server;
}

void MainWindow::logWrite(const QString& aMsg)
{
    ui->logTextEdit->append(aMsg);
    qDebug() << aMsg;
}

void MainWindow::addConnection()
{
    QSslSocket *socket = dynamic_cast<QSslSocket *>(m_server->nextPendingConnection());
    assert(socket);

    //Connect sockets signals.
    connect(socket, &QSslSocket::encrypted, [socket, this](){
        if (m_clients_certificates.contains(socket->peerCertificate()))
        {
            logWrite(QString("New connection %1:%2 ")
                                    .arg(socket->peerAddress().toString())
                                    .arg(socket->peerPort()));
            m_sockets.append(socket);
            socket->write("Hi from server.");
        }
        else
        {
            logWrite(QString("Peer ignored %1:%2 ")
                     .arg(socket->peerAddress().toString())
                     .arg(socket->peerPort()));
            socket->close();
        }
    });
    connect(socket, &QSslSocket::disconnected, [socket, this](){
        logWrite(QString("Peer %1:%2 disconnected.")
                                .arg(socket->peerAddress().toString())
                                .arg(socket->peerPort()));
        m_sockets.removeOne(socket);
    });
    connect(socket, &QSslSocket::readyRead, [socket, this](){
        logWrite(QString("Msg from %1:%2 : %3")
                                .arg(socket->peerAddress().toString())
                                .arg(socket->peerPort())
                                .arg(socket->readAll().constData()));
    });
    connect(socket, &QSslSocket::stateChanged, [this](QAbstractSocket::SocketState state){
        qDebug() << Q_FUNC_INFO << state;
    });

    connect(socket, static_cast<void (QSslSocket::*)(const QList<QSslError> &)>(&QSslSocket::sslErrors), [socket, this](const QList<QSslError> &errors){
        logWrite(QString("Errors %1:%2 :")
                 .arg(socket->peerAddress().toString())
                 .arg(socket->peerPort()));

        foreach (auto err, errors)
        {
            logWrite(err.errorString());
        }
    });

    connect(socket, static_cast<void (QSslSocket::*)(QAbstractSocket::SocketError)>(&QSslSocket::error), [this](QAbstractSocket::SocketError errors){
        ui->logTextEdit->append("Сritical error.");
        qDebug() << errors;
    });

    QList<QSslError> ignoreErrors;
    ignoreErrors << QSslError::CertificateUntrusted << QSslError::SelfSignedCertificate;
    socket->ignoreSslErrors(ignoreErrors);

    socket->setLocalCertificate(m_certificate);
    socket->setPrivateKey(m_key);
    socket->setPeerVerifyMode(QSslSocket::VerifyPeer);
    socket->startServerEncryption();
}
