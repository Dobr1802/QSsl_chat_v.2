#include "userscertificates.h"
#include <QDebug>
#include <QCoreApplication>

const QString USER_CERTS = "user_certificate/%1";
const QString USER_NUM = "usernum";

//Users certificates path HOME$/.config/RBT/QSslServer.conf
UsersCertificates::UsersCertificates() : m_settings("RBT", "QSslServer")
{
    if (m_settings.contains(USER_NUM))
        m_clientsNum = m_settings.value(USER_NUM).toInt();
    else
        m_settings.setValue(USER_NUM, 0);
    qDebug() << Q_FUNC_INFO << m_clientsNum;
}

void UsersCertificates::add(const QByteArray &cert)
{
    if (!contains(QSslCertificate(cert)))
    {
        m_settings.setValue(USER_CERTS.arg(m_clientsNum++), cert);
        m_settings.setValue(USER_NUM, m_clientsNum);
    }
    qDebug() << Q_FUNC_INFO << m_clientsNum;
}

//certificate must be in Pem encoding.
bool UsersCertificates::contains(const QSslCertificate &cert)
{
    QStringList keys = m_settings.allKeys();
    for (auto k : keys)
    {
        if (m_settings.value(k).value<QByteArray>() == cert.toPem())
            return true;
    }
    return false;
}

void UsersCertificates::removeByCertificate(const QByteArray &cert)
{
    QStringList keys = m_settings.allKeys();
    for (auto k : keys)
    {
        if (m_settings.value(k).value<QByteArray>() == cert)
        {
            m_settings.remove(k);
            m_clientsNum--;
            m_settings.setValue(USER_NUM, m_clientsNum);
            break;
        }
    }
}

void UsersCertificates::removeByKey(const QByteArray &key)
{
    removeByCertificate(m_settings.value(key).value<QByteArray>());
}

QStringList UsersCertificates::list()
{
    return m_settings.allKeys();
}
