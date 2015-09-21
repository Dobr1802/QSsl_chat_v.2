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
        qDebug() << "ok...\n";
    else
        qDebug() << "bad \n";

    m_socket = new QSslSocket();
    m_socket->setPeerVerifyMode(QSslSocket::VerifyPeer);

    m_socket->setLocalCertificate(ui->sertLineEdit->text());
    m_socket->setPrivateKey(ui->keyLineEdit->text());

    connect(m_socket, &QSslSocket::readyRead, [this](){
        ui->textEdit->setText(m_socket->readAll().constData());
    });
    connect(m_socket, SIGNAL(sslErrors(const QList<QSslError> &)), this, SLOT(sslErr(const QList<QSslError> &)));
    connect(m_socket, SIGNAL(error(QAbstractSocket::SocketError)), this, SLOT(somthWrong(QAbstractSocket::SocketError)));
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
    if ((errors.size() == 1) && (errors.first() == QSslError::SelfSignedCertificate || QSslError::CertificateUntrusted))
    {
        m_socket->ignoreSslErrors();
    }
    else
    {
        qDebug() << errors;
    }
}

void MainWindow::somthWrong(QAbstractSocket::SocketError err)
{
    qDebug() << err;
    m_socket->close();
}
