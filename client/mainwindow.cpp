#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QDebug>

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

    connect(m_socket, &QTcpSocket::readyRead, [this](){
        ui->textEdit->setText(m_socket->readAll().constData());
    } );
}

MainWindow::~MainWindow()
{
    delete ui;
    if (m_socket->isOpen())
        delete m_socket;
}

void MainWindow::on_connectButton_clicked()
{
    m_socket->setPeerVerifyMode(QSslSocket::VerifyPeer);
    m_socket->connectToHostEncrypted(ui->host->text(), ui->port->text().toInt());
}
