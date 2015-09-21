#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QSslSocket>
#include "sslserver.h"

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
    void sslErr(const QList<QSslError> &errors);

private:
    Ui::MainWindow *ui;
    SslServer *m_server;
    QList<QSslCertificate> m_clients_certificates;
    QString m_certificate, m_key;
};

#endif // MAINWINDOW_H
