#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QDebug>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    _server = new QTcpServer();
    _server->listen(QHostAddress::Any, ui->port->text().toInt());
    connect(_server, SIGNAL(newConnection()), this, SLOT(addConnection()));

    ui->startStopServer->setEnabled(false);
}

MainWindow::~MainWindow()
{
    delete ui;
    if (_socket->isOpen())
        delete _socket;
}

void MainWindow::addConnection()
{
    _socket = new QTcpSocket(_server->nextPendingConnection());
    _socket->open(QIODevice::ReadWrite);
    _socket->waitForReadyRead(-1);
    _socket->write("some text");

//    connect(_socket, SIGNAL(readyRead()), this, SLOT(write()));
}

void MainWindow::write()
{
}
