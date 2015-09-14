#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTcpServer>
#include <QTcpSocket>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    void addConnection();
    void write();

private:
    Ui::MainWindow *ui;
    QTcpServer *_server;
    QTcpSocket *_socket;
};

#endif // MAINWINDOW_H
