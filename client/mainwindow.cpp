#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QDebug>
#include <QSslKey>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    if(QSslSocket::supportsSsl())
    {
        m_socket = new QSslSocket();
        m_socket->setPeerVerifyMode(QSslSocket::VerifyPeer);

        m_socket->setLocalCertificate(ui->sertLineEdit->text());
        m_socket->setPrivateKey(ui->keyLineEdit->text());

        //Connect socket signals.
        connect(m_socket, &QSslSocket::readyRead, [this](){
            ui->logTextEdit->append(m_socket->readAll().constData());
        });
        connect(m_socket, &QSslSocket::encrypted, [this](){
            ui->logTextEdit->append(QString("Connected"));
        });
        connect(m_socket, SIGNAL(sslErrors(const QList<QSslError> &)), this, SLOT(sslErr(const QList<QSslError> &)));
        connect(m_socket, SIGNAL(error(QAbstractSocket::SocketError)), this, SLOT(somthWrong(QAbstractSocket::SocketError)));

        //Connect buttons.
        connect(ui->sendPushButton, &QAbstractButton::clicked, [this](){
            m_socket->write(QString("%1").arg(ui->sendingMsgLineEdit->text()).toUtf8());
            ui->sendingMsgLineEdit->clear();
        });
    }
    else
        ui->logTextEdit->setText("SSL does not support.");
}

MainWindow::~MainWindow()
{
    delete ui;
    if (m_socket->isOpen())
        delete m_socket;
}

void MainWindow::on_connectButton_clicked()
{
    m_socket->connectToHostEncrypted(ui->host->text(), ui->port->text().toInt());
}

void MainWindow::sslErr(const QList<QSslError> &errors)
{
    ui->logTextEdit->append(QString("Errors:"));
    foreach (auto err, errors) {
        ui->logTextEdit->append(err.errorString());
    }
    m_socket->ignoreSslErrors();
}

void MainWindow::somthWrong(QAbstractSocket::SocketError err)
{
    ui->logTextEdit->append("Connection error.");
    qDebug() << err;
}
