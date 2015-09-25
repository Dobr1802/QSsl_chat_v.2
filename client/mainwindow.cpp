#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QDebug>
#include <QSslKey>
#include <QFileDialog>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    if(!QSslSocket::supportsSsl())
    {
        ui->logTextEdit->setText("SSL does not support.");
        return;
    }

    m_socket = new QSslSocket();
    m_socket->setPeerVerifyMode(QSslSocket::VerifyPeer);

    //Connect socket signals.
    connect(m_socket, &QSslSocket::readyRead, [this](){
        ui->logTextEdit->append(m_socket->readAll().constData());
    });
    connect(m_socket, &QSslSocket::encrypted, [this](){
        ui->logTextEdit->append(QString("Connected"));
    });
    connect(m_socket, &QSslSocket::stateChanged, [this](QAbstractSocket::SocketState state){
        qDebug() << Q_FUNC_INFO << state;
    });
    connect(m_socket, SIGNAL(sslErrors(const QList<QSslError> &)), this, SLOT(sslErr(const QList<QSslError> &)));
    connect(m_socket, SIGNAL(error(QAbstractSocket::SocketError)), this, SLOT(somthWrong(QAbstractSocket::SocketError)));

    //Connect buttons.
    connect(ui->sendPushButton, &QAbstractButton::clicked, [this](){
        m_socket->write(ui->sendingMsgLineEdit->text().toUtf8());
        ui->sendingMsgLineEdit->clear();
    });
    connect(ui->certificateOpenButton, &QAbstractButton::clicked, [this](){
        QString certificate = QFileDialog::getOpenFileName(this, "Select certificate file");
        if(!certificate.isEmpty())
        {
            m_socket->setLocalCertificate(certificate);
            ui->sertLineEdit->setText(certificate);
        }
    });
    connect(ui->keyOpenButton, &QAbstractButton::clicked, [this](){
        QString key = QFileDialog::getOpenFileName(this, "Select key file");
        if(!key.isEmpty())
        {
            m_socket->setPrivateKey(key);
            ui->keyLineEdit->setText(key);
        }
    });
    connect(ui->connectButton, &QAbstractButton::clicked, [this](){
        m_socket->connectToHostEncrypted(ui->host->text(), ui->port->text().toInt());
    });
}

MainWindow::~MainWindow()
{
    delete ui;
    m_socket->deleteLater();
}

void MainWindow::sslErr(const QList<QSslError> &errors)
{
    ui->logTextEdit->append("Errors:");
    for(const auto &err: errors)
    {
        ui->logTextEdit->append(err.errorString());
    }

    QList<QSslError> ignoreErrors;
    ignoreErrors.append(QSslError::CertificateUntrusted);
    ignoreErrors.append(QSslError::SelfSignedCertificate);
    m_socket->ignoreSslErrors(ignoreErrors);
}

void MainWindow::somthWrong(QAbstractSocket::SocketError err)
{
    ui->logTextEdit->append("Connection error.");
    qDebug() << err;
}
