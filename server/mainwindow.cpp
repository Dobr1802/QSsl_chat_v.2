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

    updateUsersListWidget();

    if(!QSslSocket::supportsSsl())
    {
        ui->logTextEdit->setText("SSL does not support.");
        return;
    }

    //Connect buttons.
    //Add user certificate button.
    connect(ui->userCertButton, &QAbstractButton::clicked, [this](){
        QString path = QFileDialog::getOpenFileName(this, "Select certificate file");
        if (!path.isEmpty())
        {
            QFile file(path);
            file.open(QIODevice::ReadOnly);
            m_clients_certificates.add(file.readAll());
            file.close();

            updateUsersListWidget();
        }
    });
    //Remove user sertifivate button.
    connect(ui->removeUserPushButton, &QAbstractButton::clicked, [this](){
        if (ui->listWidget->currentItem())
        {
            m_clients_certificates.removeByKey(ui->listWidget->currentItem()->data(0).value<QByteArray>());
            updateUsersListWidget();
        }
        else
        {
            QString path = QFileDialog::getOpenFileName(this, "Select certificate file");
            if (!path.isEmpty())
            {
                QFile file(path);
                file.open(QIODevice::ReadOnly);
                m_clients_certificates.removeByCertificate(file.readAll());
                file.close();

                updateUsersListWidget();
            }
        }
    });
    //Add server certificate button.
    connect(ui->certificateOpenButton, &QAbstractButton::clicked, [this](){
        QString certificate = QFileDialog::getOpenFileName(this, "Select certificate file");
        if (!certificate.isEmpty())
        {
            ui->certLineEdit->setText(certificate);
            m_certificate = certificate;
        }
    });
    //Add server private key button.
    connect(ui->keyOpenButton, &QAbstractButton::clicked, [this](){
        QString key = QFileDialog::getOpenFileName(this, "Select key file");
        if (!key.isEmpty())
        {
            ui->keyLineEdit->setText(key);
            m_key = key;
        }
    });
    //Start server button.
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
    //Socket::encrypted
    connect(socket, &QSslSocket::encrypted, [socket, this](){
        if (m_clients_certificates.contains(socket->peerCertificate().toPem()))
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
    //Socket::disconnected
    connect(socket, &QSslSocket::disconnected, [socket, this](){
        logWrite(QString("Peer %1:%2 disconnected.")
                                .arg(socket->peerAddress().toString())
                                .arg(socket->peerPort()));
        m_sockets.removeOne(socket);
    });
    //Socket::redyRead
    connect(socket, &QSslSocket::readyRead, [socket, this](){
        logWrite(QString("Msg from %1:%2 : %3")
                                .arg(socket->peerAddress().toString())
                                .arg(socket->peerPort())
                                .arg(socket->readAll().constData()));
    });
    //Socket::stateChanged
    connect(socket, &QSslSocket::stateChanged, [this](QAbstractSocket::SocketState state){
        qDebug() << Q_FUNC_INFO << state;
    });
    //Socket::sslErrors
    connect(socket, static_cast<void (QSslSocket::*)(const QList<QSslError> &)>(&QSslSocket::sslErrors), [socket, this](const QList<QSslError> &errors){
        logWrite(QString("Errors %1:%2 :")
                 .arg(socket->peerAddress().toString())
                 .arg(socket->peerPort()));

        foreach (auto err, errors)
        {
            logWrite(err.errorString());
        }
    });
    //Socket::error
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

void MainWindow::updateUsersListWidget()
{
    ui->listWidget->clear();
    ui->listWidget->addItems(m_clients_certificates.list());
}
