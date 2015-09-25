#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QDebug>
#include <cassert>
#include <QSslCertificate>
#include <QSettings>
#include <QFile>
#include <QFileDialog>

const QString USER_CERTS = "users_certificates";

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent), ui(new Ui::MainWindow), m_server(new SslServer(this))
{
    ui->setupUi(this);

    if(!QSslSocket::supportsSsl())
    {
        ui->logTextEdit->setText("SSL does not support.");
        return;
    }

    QCoreApplication::setOrganizationName("RBT");
    QCoreApplication::setApplicationName("QSslServer");

    QSettings settings;
    if (settings.contains(USER_CERTS))
    {
        m_clients_certificates = settings.value(USER_CERTS).value<QList<QByteArray>>();
    }

    //Connect buttons.
    connect(ui->userCertButton, &QAbstractButton::clicked, [&settings, this](){
        QString path = QFileDialog::getOpenFileName(this, "Select key file");
        if (!path.isEmpty())
        {
            QFile file(path);
            file.open(QIODevice::ReadOnly);
            m_clients_certificates.append(file.readAll());
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

    connect(socket, SIGNAL(sslErrors(const QList<QSslError> &)), this, SLOT(sslErr(const QList<QSslError> &)));
    connect(socket, SIGNAL(error(QAbstractSocket::SocketError)), this, SLOT(somthWrong(QAbstractSocket::SocketError)));

    QList<QSslError> ignoreErrors;
    ignoreErrors << QSslError::CertificateUntrusted << QSslError::SelfSignedCertificate;
    socket->ignoreSslErrors(ignoreErrors);

    socket->setLocalCertificate(m_certificate);
    socket->setPrivateKey(m_key);
    socket->setPeerVerifyMode(QSslSocket::VerifyPeer);
    socket->startServerEncryption();
}

void MainWindow::sslErr(const QList<QSslError> &errors)
{
    QSslSocket *socket = dynamic_cast<QSslSocket *>(sender());
    assert(socket);

    logWrite(QString("Errors %1:%2 :")
             .arg(socket->peerAddress().toString())
             .arg(socket->peerPort()));

    foreach (auto err, errors)
    {
        logWrite(err.errorString());
    }
}

void MainWindow::somthWrong(QAbstractSocket::SocketError err)
{
    qDebug() << err;
}
