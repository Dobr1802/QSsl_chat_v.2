#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QSslSocket>

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
    void sslErr(const QList<QSslError> &err);
    void somthWrong(QAbstractSocket::SocketError err);

private:
    Ui::MainWindow *ui;
    QSslSocket *m_socket;
};

#endif // MAINWINDOW_H
