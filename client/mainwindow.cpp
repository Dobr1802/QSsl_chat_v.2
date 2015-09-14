#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    _socket = new QTcpSocket();

    connect(_socket, SIGNAL(readyRead()), this, SLOT(read()));
}

MainWindow::~MainWindow()
{
    delete ui;
    if (_socket->isOpen())
        delete _socket;
}

void MainWindow::on_connectButton_clicked()
{
    _socket->connectToHost(ui->host->text(), ui->port->text().toInt());
    _socket->open(QIODevice::ReadWrite);
    _socket->readyRead();
}

void MainWindow::read()
{
    ui->textEdit->setText(_socket->readAll().constData());
}
