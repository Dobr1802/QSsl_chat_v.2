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

    if(!QSslSocket::supportsSsl())
        ui->logTextEdit->setText("SSL does not support.");
    else
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
            if (!m_certificate.isEmpty())
            {
                ui->certLineEdit->setText(m_certificate);
            }
        });
        connect(ui->keyOpenButton, &QAbstractButton::clicked, [this](){
            m_key = QFileDialog::getOpenFileName(this, "Select keyfile");
            if (!m_key.isEmpty())
            {
                ui->keyLineEdit->setText(m_key);
            }
        });
        connect(ui->starServer, &QAbstractButton::clicked, [this](){
            if (!ui->port->text().isEmpty())
            {
                m_server->listen(QHostAddress::Any, ui->port->text().toInt());
                connect(m_server, SIGNAL(newConnection()), this, SLOT(addConnection()));
            }
        });
    }
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
    socket->setPeerVerifyMode(QSslSocket::VerifyPeer);

    connect(socket, &QSslSocket::encrypted, [socket, this](){
        foreach (QSslCertificate cert, m_clients_certificates)
        {
            if (socket->peerCertificate() == cert)
            {
                ui->logTextEdit->append(QString("New connection %1:%2 ")
                                        .arg(socket->peerAddress().toString())
                                        .arg(socket->peerPort()));
                break;
            }
            ui->logTextEdit->append(QString("Peer ignored %1:%2 ")
                                    .arg(socket->peerAddress().toString())
                                    .arg(socket->peerPort()));
            socket->close();
        }
    });

    connect(socket, SIGNAL(sslErrors(const QList<QSslError> &)), this, SLOT(sslErr(const QList<QSslError> &)));
    connect(socket, &QSslSocket::readyRead, [socket, this](){
        ui->logTextEdit->append(QString("Msg from %1:%2 : %3")
                                .arg(socket->peerAddress().toString())
                                .arg(socket->peerPort())
                                .arg(socket->readAll().constData()));
    });

    QList<QSslError> errors;
    errors.append(QSslError::CertificateUntrusted);
    errors.append(QSslError::SelfSignedCertificate);
    socket->ignoreSslErrors(errors);

    socket->setLocalCertificate(m_certificate);
    socket->setPrivateKey(m_key);
    socket->startServerEncryption();
}

void MainWindow::sslErr(const QList<QSslError> &err)
{
    qDebug() << err;
}
