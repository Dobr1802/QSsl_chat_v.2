#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QSslSocket>
#include "sslserver.h"
#include "userscertificates.h"

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

private:
    Ui::MainWindow *ui;
    SslServer *m_server;
    QList<QSslSocket *> m_sockets;
    UsersCertificates m_clients_certificates;
    QString m_certificate, m_key;

    void updateUsersListWidget();
    void logWrite(const QString &aMsg);
};

#endif // MAINWINDOW_H
