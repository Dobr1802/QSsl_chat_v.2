#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    server->listen(QHostAddress::Any, 4729);
    connect(server, SIGNAL(newConnection()), this, SLOT(addConnection()));
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::addConnection()
{

}
