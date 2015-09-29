#ifndef USERSCERTIFICATES_H
#define USERSCERTIFICATES_H

#include <QSettings>
#include <QSslCertificate>

class UsersCertificates : public QObject
{
public:
    UsersCertificates();

    void add(const QByteArray &cert);

    bool contains(const QSslCertificate &cert);
    void removeByCertificate(const QByteArray &cert);
    void removeByKey(const QByteArray &key);
    QStringList list();

private:
    int m_clientsNum = 0;
    QSettings m_settings;
};

#endif // USERSCERTIFICATES_H
